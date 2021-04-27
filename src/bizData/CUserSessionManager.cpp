
#include <fstream>
#include "bizData/CUserSessionManager.h"
#include "core/marco.h"


CUser::CUser(std::string userid, bool ifWildcard)
: UserID(userid), IfWildcardAllowed(ifWildcard)
{

}


CUserSessionManager::CUserSessionManager(int maxOnlineUser)
{   

    RWMutex::WriteLock guard(m_Mutex);

    m_UserSessionVec.reserve(maxOnlineUser);
    
    for (int i = 0; i < maxOnlineUser; ++i)
    {   
        UserSession userSession;
        userSession.IfSubsAll = false;
        userSession.IfUnsubAll = false;
        userSession.IfWildcard = false;
        userSession.SessionID = 0;
        userSession.Status = (char) UserSessionStatus::Invalid;
        userSession.Timestamp = 0;
        m_UserSessionVec.push_back(userSession);
    }


    /// 从文件中读取授权过的userid
    auto authorized_user_count = LoadAuthorizedUsersFromFile();

    SPDLOG_INFO("Load {} authorized users  from file", authorized_user_count);

    
}

CUserSessionManager::~CUserSessionManager()
{
    SPDLOG_INFO("CUserSessionManager deConstruction");
}


int CUserSessionManager::LoadAuthorizedUsersFromFile()
{
    std::ifstream ifs("./config/authorized_users.csv");
    if (ifs)
    {   
        std::string userid;
        bool IfWildcard;
        while(ifs>>userid >> IfWildcard)
        {
            m_AuthorizedUsersVec.emplace_back(userid, IfWildcard);
        }
    }
    else
    {
        SPDLOG_ERROR("Failed to open ./config/authorized_users.csv!");
    }

    return m_AuthorizedUsersVec.size();
}



bool CUserSessionManager::CheckIfFull()
{
    RWMutex::ReadLock guard(m_Mutex);

    for (auto session: m_UserSessionVec)
    {
        if (session.Status!= (char)UserSessionStatus::Login)
        {
            return false;
        }
    }
    return true;
}


bool CUserSessionManager::CheckIfNewSession(const UserSessionIdType& id)
{
    RWMutex::ReadLock guard(m_Mutex);

    auto iter = std::find_if(m_UserSessionVec.begin(), m_UserSessionVec.end(), [&](const UserSession& t){return t.SessionID == id;});
    if (MY_LIKELY(iter == m_UserSessionVec.end()))
    {
        return true;
    }
    return false;
}


bool CUserSessionManager::CheckIfAuthorized(const std::string& UserID)
{   
    /// 初始化时从内存读取，后续无更新
    auto iter = std::find_if(m_AuthorizedUsersVec.begin(), m_AuthorizedUsersVec.end(), [&](const CUser&  user){return user.UserID == UserID;});
    if (iter == m_AuthorizedUsersVec.end())
    {
        return false;
    }
    return true;
}

bool CUserSessionManager::GetIfWildcard(const std::string& UserID)
{   
    /// 初始化时从内存读取，后续无更新
    auto iter = std::find_if(m_AuthorizedUsersVec.begin(), m_AuthorizedUsersVec.end(), [&](const CUser&  user){return user.UserID == UserID;});
    if (iter == m_AuthorizedUsersVec.end())
    {
        return false;
    }
    return iter->IfWildcardAllowed;
}

bool CUserSessionManager::CheckIfWildcard(const UserSessionIdType &id)
{
    /// 初始化时从内存读取，后续无更新
    auto iter = std::find_if(
        m_UserSessionVec.begin(), m_UserSessionVec.end(), [&](const UserSession &t) { return t.SessionID == id; });
    if (MY_UNLIKELY(iter == m_UserSessionVec.end()))
    {
        return false;
    }
    return iter->IfWildcard;
}

bool CUserSessionManager::AddUserSession(SessionIdType sessionid, bool if_wildcard)
{
    RWMutex::ReadLock guard(m_Mutex);
    auto iter = std::find_if(m_UserSessionVec.begin(), m_UserSessionVec.end(), [&](const UserSession& t){return t.Status != (char)UserSessionStatus::Login;});
    if (MY_UNLIKELY(iter == m_UserSessionVec.end()))
    {
        return false;
    }


    iter->SessionID = sessionid;
    iter->Status = (char) UserSessionStatus::Login;
    iter->IfWildcard = if_wildcard;

    time_t timestamp;
    iter->Timestamp = time(&timestamp);

    return true;
}


int CUserSessionManager::CheckIfLogin(const UserSessionIdType &id)
{
    RWMutex::ReadLock guard(m_Mutex);

    auto iter = std::find_if(m_UserSessionVec.begin(), m_UserSessionVec.end(), 
        [&](const UserSession& t){return (t.SessionID == id) && (t.Status == (char)UserSessionStatus::Login);});
    
    if (MY_LIKELY(iter != m_UserSessionVec.end()))
    {
        return iter - m_UserSessionVec.begin();
    }

    return COMMON_FAILURE;
}



bool CUserSessionManager::SubscribeAllBySessionID(SessionIdType sessionID) 
{
    RWMutex::WriteLock guard(m_Mutex);

    auto iter = std::find_if(m_UserSessionVec.begin(), m_UserSessionVec.end(), [&](const UserSession& t){return t.SessionID == sessionID;});
    if (MY_UNLIKELY(iter == m_UserSessionVec.end()))
    {
        SPDLOG_ERROR("Failed to find sesssionid {}", sessionID);
        return false;
    }

    iter->IfSubsAll = true;
    iter->IfUnsubAll = false;

    return true;
}

bool CUserSessionManager::UnSubscribeAllBySessionID(SessionIdType sessionID)
{
    RWMutex::WriteLock  guard(m_Mutex);

    auto iter = std::find_if(
        m_UserSessionVec.begin(), m_UserSessionVec.end(), [&](const UserSession &t) { return t.SessionID == sessionID; });
    if (MY_UNLIKELY(iter == m_UserSessionVec.end()))
    {
        SPDLOG_ERROR("Failed to find sesssionid {}", sessionID);
        return false;
    }

    iter->IfSubsAll = false;

    return true;
}

void CUserSessionManager::CheckIfSubs(std::bitset<MAX_ONLINE_USERS> &subscribers, std::set<SessionIdType>& dispatchSet) {

    RWMutex::ReadLock  guard(m_Mutex);

    for (int i = 0; i < MAX_ONLINE_USERS; ++i)
    {
        auto if_subs_all = m_UserSessionVec[i].IfSubsAll;
        auto sub =  subscribers[i];
        if (if_subs_all || sub == true)
        {
            dispatchSet.insert(m_UserSessionVec[i].SessionID);
        }
    }
}

bool CUserSessionManager::HandleUserLogout(SessionIdType sessionID)
{

    RWMutex::WriteLock  guard(m_Mutex);

    auto iter = std::find_if(
        m_UserSessionVec.begin(), m_UserSessionVec.end(), [&](const UserSession &t) { return t.SessionID == sessionID; });


    if (MY_UNLIKELY(iter==m_UserSessionVec.end()))
    {
        return false;
    }

    iter->IfSubsAll = false;
    iter->IfUnsubAll = true;
    iter->Status = (char) UserSessionStatus::Logout;

    return true;
}

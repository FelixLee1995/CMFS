#include "plugin/CUserSessionManager.h"
#include <fstream>



CUserSessionManager::CUserSessionManager(int maxOnlineUser)
{
    m_UserSessionVec.reserve(maxOnlineUser);
    
    for (int i = 0; i < maxOnlineUser; ++i)
    {   
        UserSession userSession;
        userSession.IfSubsAll = false;
        userSession.IfUnsubAll = false;
        userSession.SessionID = 0;
        userSession.Status = (char) UserSessionStatus::Invalid;
        userSession.Timestamp = 0;
        userSession.UserSessionID = 0;
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
        while(ifs>>userid)
        {
            m_AuthorizedUsersVec.push_back(userid);
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

    auto iter = std::find_if(m_UserSessionVec.begin(), m_UserSessionVec.end(), [&](const UserSession& t){return t.SessionID == id;});
    if (iter == m_UserSessionVec.end())
    {
        return true;
    }
    return false;
}


bool CUserSessionManager::CheckIfAuthorized(const std::string& UserID)
{
    auto iter = std::find_if(m_AuthorizedUsersVec.begin(), m_AuthorizedUsersVec.end(), [&](const std::string& id){return id == UserID;});
    if (iter == m_AuthorizedUsersVec.end())
    {
        return false;
    }
    return true;
}

bool CUserSessionManager::AddUserSession(SessionIdType sessionid)
{
    auto iter = std::find_if(m_UserSessionVec.begin(), m_UserSessionVec.end(), [&](const UserSession& t){return t.Status != (char)UserSessionStatus::Login;});
    if (iter == m_UserSessionVec.end())
    {
        return false;
    }


    iter->SessionID = sessionid;
    iter->Status = (char) UserSessionStatus::Login;

    time_t timestamp;
    iter->Timestamp = time(&timestamp);

    return true;
}


int CUserSessionManager::CheckIfLogin(const UserSessionIdType &id)
{
    auto iter = std::find_if(m_UserSessionVec.begin(), m_UserSessionVec.end(), 
        [&](const UserSession& t){return (t.SessionID == id) && (t.Status == (char)UserSessionStatus::Login);});
    
    if (iter != m_UserSessionVec.end())
    {
        return iter - m_UserSessionVec.begin();
    }

    return -1;
}



bool CUserSessionManager::SubscribeAllBySessionID(SessionIdType sessionID) 
{

    auto iter = std::find_if(m_UserSessionVec.begin(), m_UserSessionVec.end(), [&](const UserSession& t){return t.SessionID == sessionID;});
    if (iter == m_UserSessionVec.end())
    {
        SPDLOG_ERROR("Failed to find sesssionid {}", sessionID);
        return false;
    }

    iter->IfSubsAll = true;
    iter->IfUnsubAll = false;

    return true;
}
void CUserSessionManager::CheckIfSubs(std::bitset<MAX_ONLINE_USERS> &subscribers, std::set<SessionIdType>& dispatchSet) {
    for (int i = 0; i < MAX_ONLINE_USERS; ++i)
    {
        auto if_subs_all = m_UserSessionVec[i].IfSubsAll;
        auto if_un_subs_all =  m_UserSessionVec[i].IfUnsubAll;
        auto sub =  subscribers[i];
        if (!if_un_subs_all && (if_subs_all || sub == true))
        {
            dispatchSet.insert(m_UserSessionVec[i].SessionID);
        }
    }
}

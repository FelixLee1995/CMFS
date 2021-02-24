//
// Created by felix on 2021/2/9.
//

#include "plugin/CUserManagePlugin.h"
#include "api/ctp/ThostFtdcMdApi.h"
#include <netinet/in.h>

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
    return m_UserSessionVec.size() >= MAX_ONLINE_USERS;
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
    auto iter = std::find(m_UserSessionVec.begin(), m_UserSessionVec.end(), [&](const UserSession& t){return t.Status != (char)UserSessionStatus::Login;});
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




CUserManagePlugin::CUserManagePlugin(): IPlugin("UserManagePlugin")
{

    m_UserSessionManager.reset(Singleton<CUserSessionManager>::Instance(MAX_ONLINE_USERS));


    m_TcpServer.reset(Singleton<CTcpServer>::GetInstance());

}

void CUserManagePlugin::Init()
{
    Subscribe(TOPIC_USER_MANAGE, FUNC_REQ_USER_LOGIN);
    Subscribe(TOPIC_USER_MANAGE, FUNC_REQ_USER_LOGOUT);

}
void CUserManagePlugin::MsgHandler(const Msg &msg) {
    switch(msg.Header.FuncId) {
        case FUNC_REQ_USER_LOGIN: {
            HandleUserLogin(msg);
            break;
        }
        case FUNC_REQ_USER_LOGOUT: {
            HandleUserLogout(msg);
            break;
        }
        default: {
            SPDLOG_ERROR("NO HANDLER FOR FUNCID: {}", msg.Header.FuncId);
            return;
        }
    }
}
void CUserManagePlugin::HandleUserLogin(const Msg &msg)
{
    SPDLOG_INFO("handle user login");

    auto ret = CheckLoginUser(msg);

    //res pair
    //ErrorEngine


    
    m_TcpServer->PubBizMsg();

    /// 5. 生成登录应答

    return;

}


int CUserManagePlugin::CheckLoginUser(const Msg &msg)
{
    /// 1.  检查MaxOnlineUsers是否超过 设定
    auto if_full = m_UserSessionManager->CheckIfFull();
    if (if_full)
    {
        ///todo 返回用户会话超过上限的消息
        SPDLOG_INFO("usersession full!");

        return (int)LoginErrorID::FullOfUsers;
    }

    /// 2. 业务包解包, 检查sessionID是否用过
    
    auto field = (ftdc_field_header*) msg.Pack;
    auto loginField = (CThostFtdcReqUserLoginField *)  (msg.Pack + sizeof(ftdc_field_header));
    auto sessionId = msg.Header.SessionId;
    SPDLOG_INFO("sessionid is {}, contentLen is {}, field[0].topicid is {}, field[0].size is {},  userID is [{}]",
        sessionId, msg.Header.ContentLen, ntohs(field->field_id), ntohs(field->field_len), loginField->UserID);



    auto if_new_session = m_UserSessionManager->CheckIfNewSession(sessionId);
    
    if (!if_new_session)
    {
        SPDLOG_ERROR("session {} already in use!", sessionId);
        return (int)LoginErrorID::SessionAlreadyInUse;
    }


    /// 3. 校验密码    
    /// todo  用户校验拟采用文件形式存储， 可只校验用户号

    auto if_authorized = m_UserSessionManager->CheckIfAuthorized(loginField->UserID);
    
    if (!if_authorized)
    {
        SPDLOG_ERROR("UserID {} Unauthorized!", loginField->UserID);
        return (int)LoginErrorID::Unauthorized;
    }


    /// 4. 生成新的UserSession, 并放入vector

    auto if_session_op = m_UserSessionManager->AddUserSession(sessionId);
    if (!if_session_op)
    {
        SPDLOG_ERROR("Failed to addUserSession {}", sessionId);
        return (int)LoginErrorID::UserSessionOpFailed;
    }


    return LoginErrorID::OK;
}



void CUserManagePlugin::HandleUserLogout(const Msg &msg) {}


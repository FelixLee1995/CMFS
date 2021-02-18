//
// Created by felix on 2021/2/9.
//

#include "plugin/CUserManagePlugin.h"
#include "api/ctp/ThostFtdcMdApi.h"
#include <netinet/in.h>

CUserSessionManager::CUserSessionManager(int maxOnlineUser)
{
    m_UserSessionVec.reserve(maxOnlineUser);
    
}

CUserSessionManager::~CUserSessionManager()
{
    SPDLOG_INFO("CUserSessionManager deConstruction");
}


bool CUserSessionManager::CheckIfFull()
{
    return m_UserSessionVec.size() >= MAX_ONLINE_USERS;
}


bool CUserSessionManager::CheckIfNewSession(UserSessionIdType id)
{

    auto iter = std::find_if(m_UserSessionVec.cbegin(), m_UserSessionVec.cend(), finder_t(id));
    if (iter != m_UserSessionVec.end())
    {
        return false;
    }
    return true;
}




CUserManagePlugin::CUserManagePlugin(): IPlugin("UserManagePlugin")
{

    m_UserSessionManager.reset(Singleton<CUserSessionManager>::Instance(MAX_ONLINE_USERS));

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

    /// 1.  检查MaxOnlineUsers是否超过 设定
    auto if_full = m_UserSessionManager->CheckIfFull();
    if (if_full)
    {
        ///todo 返回用户会话超过上限的消息
        SPDLOG_INFO("usersession full!");

        return;
    }

    /// 2. 业务包解包, 检查sessionID是否用过
    
    auto field = (ftdc_field_header*) msg.Pack;
    auto loginField = (CThostFtdcReqUserLoginField *)  (msg.Pack + sizeof(ftdc_field_header));
    auto sessionId = msg.Header.SessionId;
    SPDLOG_INFO("sessionid is {}, contentLen is {}, field[0].topicid is {}, field[0].size is {},  userID is {}]",
        sessionId, msg.Header.ContentLen, ntohs(field->field_id), ntohs(field->field_len), loginField->UserID);



    auto if_new_session = m_UserSessionManager->CheckIfNewSession(sessionId);
    
    if (!if_new_session)
    {
        SPDLOG_ERROR("session {} already in use!", sessionId);
        return;
    }


    /// 3. 校验密码    
    /// todo  用户校验拟采用文件形式存储， 可只校验用户号

    /// 4. 生成新的UserSession, 并放入vector

    m_User


    return;

}
void CUserManagePlugin::HandleUserLogout(const Msg &msg) {}


//
// Created by felix on 2021/2/9.
//

#include "plugin/CUserManagePlugin.h"
#include "api/ctp/ThostFtdcMdApi.h"
#include "core/baseheader.h"
#include "utils/Utils.h"

CUserManagePlugin::CUserManagePlugin(): IPlugin("UserManagePlugin")
{

    m_UserSessionManager.reset(Singleton<CUserSessionManager>::GetInstance());
    m_TcpServer.reset(Singleton<CTcpServer>::GetInstance());

}

void CUserManagePlugin::Init()
{
    Subscribe(TOPIC_USER_MANAGE, FUNC_REQ_USER_LOGIN);
    Subscribe(TOPIC_USER_MANAGE, FUNC_REQ_USER_LOGOUT);
    Subscribe(TOPIC_USER_MANAGE, FUNC_REQ_USER_DISCONNECT);

    

}
void CUserManagePlugin::MsgHandler(const Msg &msg) {
    switch(msg.Header.FuncId) {
        case FUNC_REQ_USER_LOGIN: {
            HandleUserLogin(msg);
            break;
        }
        case FUNC_REQ_USER_DISCONNECT:
        {
            HandleUserDisconnect(msg);
            break;
        }
        case FUNC_REQ_USER_LOGOUT:
        {
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


    /// 1. 业务包解包
    
    auto loginField = (CThostFtdcReqUserLoginField *)  (msg.Pack + sizeof(ftdc_field_header));
    auto sessionId = msg.Header.SessionId;

    auto ret = CheckLoginUser(sessionId, loginField);



    CThostFtdcRspUserLoginField specRspInfoField = {{}};

    strcpy(specRspInfoField.BrokerID, loginField->BrokerID);
    strcpy(specRspInfoField.UserID, loginField->UserID);
    std::string timestr = GetFormatTimeStr9();
    strcpy(specRspInfoField.LoginTime, timestr.c_str());
    strcpy(specRspInfoField.TradingDay, "20210226");    // TODO  交易日真实填充
    strcpy(specRspInfoField.SystemName, "CMFS");
    specRspInfoField.FrontID = htonl(10);
    specRspInfoField.SessionID = htonl(46);

    m_TcpServer->SendRspFtdc(TOPIC_USER_MANAGE, sessionId, ftdc_tid_RspUserLogin,
        reinterpret_cast<const char *>(&specRspInfoField), sizeof(CThostFtdcRspUserLoginField), ftdc_fid_RspUserLoginField,
        std::get<0>(ret), std::get<1>(ret).c_str());
    return;
}





std::tuple<int, std::string> CUserManagePlugin::CheckLoginUser(int sessionId, const CThostFtdcReqUserLoginField* reqField)
{
    /// 1.  检查MaxOnlineUsers是否超过 设定
    auto if_full = m_UserSessionManager->CheckIfFull();
    if (if_full)
    {
        ///todo 返回用户会话超过上限的消息
        SPDLOG_INFO("usersession full!");

        return std::make_tuple((int)LoginErrorID::FullOfUsers, "UserSession Full!");
    }

    



    auto if_new_session = m_UserSessionManager->CheckIfNewSession(sessionId);
    
    if (!if_new_session)
    {
        SPDLOG_ERROR("session {} already in use!", sessionId);
        return std::make_tuple((int)LoginErrorID::SessionAlreadyInUse, "sessionAlready in use, no need to login");
    }


    /// 3. 校验密码    
    /// todo  用户校验拟采用文件形式存储， 可只校验用户号

    auto if_authorized = m_UserSessionManager->CheckIfAuthorized(reqField->UserID);
    
    if (!if_authorized)
    {
        SPDLOG_ERROR("UserID {} Unauthorized!", reqField->UserID);
        return std::make_tuple((int)LoginErrorID::Unauthorized, "unauthorized user!");
    }

    auto if_wildcard = m_UserSessionManager->GetIfWildcard(reqField->UserID);


    /// 4. 生成新的UserSession, 并放入vector

    auto if_session_op = m_UserSessionManager->AddUserSession(sessionId, if_wildcard);
    if (!if_session_op)
    {
        SPDLOG_ERROR("Failed to addUserSession {}", sessionId);
        return std::make_tuple((int)LoginErrorID::UserSessionOpFailed, "Failed to add Usersession!");
    }

    SPDLOG_INFO("user login success, sessionid: {}", sessionId);
    return std::make_tuple((int)LoginErrorID::OK, "Login Success!");
}



void CUserManagePlugin::HandleUserLogout(const Msg &msg) 
{

}


void CUserManagePlugin::HandleUserDisconnect(const Msg& msg)
{
    m_UserSessionManager->HandleUserLogout(msg.Header.SessionId);
    SPDLOG_INFO("user disconnect, sessionid: {}", msg.Header.SessionId);

    /// TODO reset marketDataManager中 各个item 的subscribers中相应的Index
}

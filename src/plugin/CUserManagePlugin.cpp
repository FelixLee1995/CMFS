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


    /// 1. 业务包解包
    
    auto loginField = (CThostFtdcReqUserLoginField *)  (msg.Pack + sizeof(ftdc_field_header));
    auto sessionId = msg.Header.SessionId;




    auto ret = CheckLoginUser(sessionId, loginField);

    //res pair
    //ErrorEngine





    char oriData[2048] = {0};
    unsigned int oriLen = 2*sizeof(ftdc_field_header) + sizeof(CThostFtdcRspInfoField) + sizeof(CThostFtdcRspUserLoginField) + sizeof(ftdc_header);
    char encodedData[2048] = {0};
    unsigned int encodedLen = 0;
    char* data =  oriData;

    auto  ftdc_header_s = (ftdc_header *)data;
    ftdc_header_s->ftdc_version = ftdc_version_cur;
    ftdc_header_s->ftdc_chain = ftd_chain_last;
    ftdc_header_s->ftdc_topic_id = htonl(ftdc_tid_RspUserLogin);
    ftdc_header_s->ftdc_seq_series = htons(ftdc_sid_none);
    ftdc_header_s->ftdc_seq_no = htonl(0);
    ftdc_header_s->ftdc_field_count = htons(2);
    ftdc_header_s->ftdc_content_len = htons(2*sizeof(ftdc_field_header) + sizeof(CThostFtdcRspInfoField) + sizeof(CThostFtdcRspUserLoginField));
    ftdc_header_s->ftdc_req_id = htonl(0);

    data += sizeof(ftdc_header);

    auto header1 = (ftdc_field_header*) data;
    header1->field_id = htons(ftdc_fid_RspInfoField);
    header1->field_len = htons(sizeof(CThostFtdcRspInfoField));

    data += sizeof(ftdc_field_header);

    auto rspInfoField = (CThostFtdcRspInfoField*) data;

    rspInfoField->ErrorID = htonl(ret);
    strcpy(rspInfoField->ErrorMsg, "LoginTest");

    data += sizeof(CThostFtdcRspInfoField);

    auto header2 = (ftdc_field_header *)data;
    header2->field_id = htons(ftdc_fid_RspUserLoginField);
    header2->field_len = htons(sizeof(CThostFtdcRspUserLoginField));

    data += sizeof(ftdc_field_header);

    auto loginRspField = (CThostFtdcRspUserLoginField *)data;

    strcpy(loginRspField->BrokerID, loginField->BrokerID);
    strcpy(loginRspField->UserID, loginField->UserID);
    strcpy(loginRspField->LoginTime, "17:40:00");
    strcpy(loginRspField->TradingDay, "20210226");
    strcpy(loginRspField->SystemName, "CMFS");
    loginRspField->FrontID = htonl(10);
    loginRspField->SessionID = htonl(46);


    EncodeZero(oriData, oriLen, encodedData +  sizeof(ftd_header) + sizeof(ftdc_crpheader), &encodedLen);

    auto ftd_header_s = (ftd_header*) encodedData;
    ftd_header_s->ftd_type = ftd_type_compressed;
    ftd_header_s->ftd_ext_len = 0;
    ftd_header_s->ftd_content_len = htons(encodedLen + sizeof(ftdc_crpheader));

    auto ftdc_crpheader_s = (ftdc_crpheader*)(encodedData + sizeof(ftd_header));
    ftdc_crpheader_s->protocol = ftdc_crp_protocol;
    ftdc_crpheader_s->method = ftdc_cmp_compresszero;

    auto senLen = encodedLen + sizeof(ftd_header) + sizeof(ftdc_crpheader);

    m_TcpServer->SendMsg(encodedData, senLen, sessionId, TOPIC_USER_MANAGE);

    /// 5. 生成登录应答

    return;

}





int CUserManagePlugin::CheckLoginUser(int sessionId, const CThostFtdcReqUserLoginField* reqField)
{
    /// 1.  检查MaxOnlineUsers是否超过 设定
    auto if_full = m_UserSessionManager->CheckIfFull();
    if (if_full)
    {
        ///todo 返回用户会话超过上限的消息
        SPDLOG_INFO("usersession full!");

        return (int)LoginErrorID::FullOfUsers;
    }

    



    auto if_new_session = m_UserSessionManager->CheckIfNewSession(sessionId);
    
    if (!if_new_session)
    {
        SPDLOG_ERROR("session {} already in use!", sessionId);
        return (int)LoginErrorID::SessionAlreadyInUse;
    }


    /// 3. 校验密码    
    /// todo  用户校验拟采用文件形式存储， 可只校验用户号

    auto if_authorized = m_UserSessionManager->CheckIfAuthorized(reqField->UserID);
    
    if (!if_authorized)
    {
        SPDLOG_ERROR("UserID {} Unauthorized!", reqField->UserID);
        return (int)LoginErrorID::Unauthorized;
    }


    /// 4. 生成新的UserSession, 并放入vector

    auto if_session_op = m_UserSessionManager->AddUserSession(sessionId);
    if (!if_session_op)
    {
        SPDLOG_ERROR("Failed to addUserSession {}", sessionId);
        return (int)LoginErrorID::UserSessionOpFailed;
    }


    return (int)LoginErrorID::OK;
}



void CUserManagePlugin::HandleUserLogout(const Msg &msg) {}


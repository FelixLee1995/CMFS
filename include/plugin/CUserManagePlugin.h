/**
* File: CUserManagePlugin.h
* Desc: 用户管理插件， 功能为处理用户会话， 登录登出
* Created by felix on 2021/2/9
*/

#ifndef USER_MANAGE_PLUGIN_H
#define USER_MANAGE_PLUGIN_H
#pragma once


#include <vector>
#include "interface/IPlugin.h"
#include "ds/bizDataStruct.h"
#include "api/ctp_ext/ctp_ftdc_proto.h"
#include "tcp/CTcpServer.h"
#include "plugin/CUserSessionManager.h"




using namespace ctp_ftd;




class CUserManagePlugin: public IPlugin
{
private:
    CUserSessionManager::Ptr m_UserSessionManager;
    CTcpServer::Sptr m_TcpServer;
    enum class LoginErrorID
    {
        OK = 0,
        FullOfUsers = 1,
        SessionAlreadyInUse = 2,
        Unauthorized = 3,
        UserSessionOpFailed = 4
    };

public:
    CUserManagePlugin();
    void Init() override;
    void MsgHandler(const Msg &msg) override;
    void HandleUserLogin(const Msg &msg);
    void HandleUserLogout(const Msg &msg);
    void HandleUserDisconnect(const Msg& msg);
    std::tuple<int, std::string> CheckLoginUser(int sessionID, const CThostFtdcReqUserLoginField* reqField);

};


#endif
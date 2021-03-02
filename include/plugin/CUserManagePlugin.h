/**
* File: CUserManagePlugin.h
* Desc: 用户管理插件， 功能为处理用户会话， 登录登出
* Created by felix on 2021/2/9
*/

#ifndef USER_MANAGE_PLUGIN_H
#define USER_MANAGE_PLUGIN_H
#pragma once


#include "interface/IPlugin.h"
#include "ds/bizDataStruct.h"
#include "api/ctp_ext/ctp_ftdc_proto.h"
#include "tcp/CTcpServer.h"

#include <vector>

//todo 修改为配置文件配置
#define MAX_ONLINE_USERS 1024

using namespace ctp_ftd;


typedef struct finder_t
{
	finder_t(UserSessionIdType n) : SessionID(n) { }
	bool operator()(const UserSession another)
	{
		return (SessionID == another.UserSessionID);
	}
	UserSessionIdType SessionID;
}finder_t;


typedef struct finder_user_t
{
	finder_user_t(std::string userid) : UserID(userid) { }
	bool operator()(const std::string& another)
	{
		return (UserID == another);
	}
	std::string UserID;
}finder_user_t;



class CUserSessionManager
{
    private:

    std::vector<struct UserSession> m_UserSessionVec;
    std::vector<std::string> m_AuthorizedUsersVec;


    int LoadAuthorizedUsersFromFile();


    public:
    using Ptr = std::shared_ptr<CUserSessionManager>;
    explicit CUserSessionManager(int MaxOnlineUser);

    /// 检查用户数是否已经满了
    bool CheckIfFull();

    /// 检查session是否是新的
    bool CheckIfNewSession(const UserSessionIdType& id);

    bool CheckIfAuthorized(const std::string& UserID);

    bool AddUserSession(SessionIdType);

    ~CUserSessionManager();


};




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
    int CheckLoginUser(int sessionID, const CThostFtdcReqUserLoginField* reqField);

};


#endif
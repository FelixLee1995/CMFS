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



class CUserSessionManager
{
    private:

    std::vector<struct UserSession> m_UserSessionVec;
    

    public:
    using Ptr = std::shared_ptr<CUserSessionManager>;
    explicit CUserSessionManager(int MaxOnlineUser);

    /// 检查用户数是否已经满了
    bool CheckIfFull();

    /// 检查session是否是新的
    bool CheckIfNewSession(UserSessionIdType id);

    ~CUserSessionManager();


};




class CUserManagePlugin: public IPlugin
{
private:
    CUserSessionManager::Ptr m_UserSessionManager;

public:
    CUserManagePlugin();
    void Init() override;
    void MsgHandler(const Msg &msg) override;
    void HandleUserLogin(const Msg &msg);
    void HandleUserLogout(const Msg &msg);

};


#endif
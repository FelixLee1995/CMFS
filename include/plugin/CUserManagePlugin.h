/**
* File: CUserManagePlugin.h
* Desc: 用户管理插件， 功能为处理用户会话， 登录登出
* Created by felix on 2021/2/9
*/

#ifndef USER_MANAGE_PLUGIN_H
#define USER_MANAGE_PLUGIN_H
#pragma once


#include "interface/IPlugin.h"

class CUserManagePlugin: public IPlugin
{
public:
    CUserManagePlugin();
    void Init() override;
    void MsgHandler(const Msg &msg) override;
    void HandleUserLogin(const Msg &msg);
    void HandleUserLogout(const Msg &msg);

};


#endif
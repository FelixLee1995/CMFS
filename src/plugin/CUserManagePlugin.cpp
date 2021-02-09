//
// Created by felix on 2021/2/9.
//

#include "plugin/CUserManagePlugin.h"

CUserManagePlugin::CUserManagePlugin(): IPlugin("UserManagePlugin")
{}

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





}
void CUserManagePlugin::HandleUserLogout(const Msg &msg) {}


/**
*  File: CUserSessionManager.h
*  Desc: 用户会话管理器
*  Created by felix on 2021/03/02
*/

#ifndef _USER_SESSION_MANAGER_H_
#define _USER_SESSION_MANAGER_H_
#pragma once


#include <set>
#include "core/mutex.h"
#include "core/baseheader.h"


class CUser
{
    public:
    bool IfWildcardAllowed;
    std::string UserID;

    CUser(std::string, bool);
};

class CUserSessionManager
{
private:
    std::vector<struct UserSession> m_UserSessionVec;
    std::vector<CUser> m_AuthorizedUsersVec;

    int LoadAuthorizedUsersFromFile();

    RWMutex m_Mutex;

public:
    using Ptr = std::shared_ptr<CUserSessionManager>;
    explicit CUserSessionManager(int maxOnlineUser);

    /// 检查用户数是否已经满了
    bool CheckIfFull();

    /// 检查session是否是新的
    bool CheckIfNewSession(const UserSessionIdType &id);

    /// 检查该用户是否授权
    bool CheckIfAuthorized(const std::string &userId);

    /// 获取session是否具有通配订阅的权限
    bool GetIfWildcard(const std::string &userId);

    /// 检查是否可以使用通配订阅
    bool CheckIfWildcard(const UserSessionIdType &id);

    /// 返回值为UserSessionVec中的index, 用于直接操作行情的subscribers表
    int CheckIfLogin(const UserSessionIdType &id);

    /// 根据subscribers列表， 计算出需要推送的用户set
    void CheckIfSubs(std::bitset<MAX_ONLINE_USERS>& subscribers, std::set<SessionIdType>&);

    bool AddUserSession(SessionIdType sessionID, bool if_wildcard);

    bool SubscribeAllBySessionID(SessionIdType sessionID);

    bool UnSubscribeAllBySessionID(SessionIdType sessionID);

    //处理用户登出
    bool HandleUserLogout(SessionIdType sessionID);

    ~CUserSessionManager();
};

#endif
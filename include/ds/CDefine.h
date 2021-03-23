//
// Created by felix on 2020/9/21.
//  用于定义基础宏
//



#ifndef CDEFINE_H
#define CDEFINE_H

#pragma once

#include <cstdint>


//todo 修改为配置文件配置
#define MAX_ONLINE_USERS 1024

using FUNCID_TYPE = uint16_t;
using TOPICID_TYPE = uint16_t;

/// topic define
const TOPICID_TYPE TOPIC_MARKET_PROCESS = 0x1100;   /// 行情处理主题号
const TOPICID_TYPE TOPIC_USER_MANAGE = 0x1200;      /// 用户管理主题号



const FUNCID_TYPE FUNC_REQ_MARKET_SUB = 0x2001;     /// 客户端请求行情订阅
const FUNCID_TYPE FUNC_REQ_MARKET_UNSUB = 0x2002;   /// 客户端请求行情退订
const FUNCID_TYPE FUNC_REQ_MARKET_SNAPSHOT_RTN = 0x2010;    /// 发送行情快照推送

const FUNCID_TYPE FUNC_REQ_USER_LOGIN = 0x3001;     /// 客户端请求登录
const FUNCID_TYPE FUNC_REQ_USER_LOGOUT = 0x3002;     /// 客户端请求登出
const FUNCID_TYPE FUNC_REQ_USER_TIMEOUT = 0x3003;     /// 客户端连接超时
const FUNCID_TYPE FUNC_REQ_USER_DISCONNECT = 0x3004;     /// 客户端连接断开




//error code define
#define COMMON_OK  0;
#define COMMON_FAILURE  -1;






#endif //CDEFINE_H

/**
*  File: bizDataStruct.h
*  Desc: 基础业务数据结构
*  Created by felix on 2021/02/05
*/

#ifndef BIZ_DATA_STRUCT_H
#define BIZ_DATA_STRUCT_H
#pragma once

#include "core/CBaseType.h"
#include "api/ctp/ThostFtdcUserApiStruct.h"



using byte = unsigned char;
using UserSessionIdType = uint8_t;
using SessionIdType = uint32_t;


enum UserSessionStatus
{
    Login = '0',
    Timeout = '1',
    Invalid = '2'
};


struct UserSession
{
    bool IfSubsAll;   /// 是否订阅全部
    bool IfUnsubAll;  /// 是否退订全部
    char Status;      ///登录状态
    UserSessionIdType UserSessionID;      /// userSessionID
    SessionIdType SessionID;     /// 套接字FD
};



///  行情的内存结构     总表为产品，  每个产品有个子表， 存储合约级别 MarketData
///  
struct MarketData
{
    byte subscribers[128];    //
    CThostFtdcDepthMarketDataField data;
};




#endif

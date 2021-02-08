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



using UserSessionIdType = uint8_t;

struct UserSession
{
    bool IfSubsAll;   /// 是否订阅全部
    bool IfUnsubAll;  /// 是否退订全部
    char Status;      ///登录状态
    UserSessionIdType UserSessionID;      /// userSessionID
    int Fd;     /// 套接字FD
};



///  行情的内存结构     总表为产品，  每个产品有个子表， 存储合约级别 MarketData
///  

struct MarketData
{
    std::byte subscribers[128];    //
    CThostFtdcDepthMarketDataField data;
};




#endif

/**
*  File: bizDataStruct.h
*  Desc: 基础业务数据结构
*  Created by felix on 2021/02/05
*/

#ifndef BIZ_DATA_STRUCT_H
#define BIZ_DATA_STRUCT_H
#pragma once


#include <bitset>
#include "ds/common.h"
#include "core/CBaseType.h"
#include "api/ctp/ThostFtdcUserApiStruct.h"

const std::string ExchangeSSE = "SSE";
const std::string ExchangeSZSE = "SZSE";
const std::string ExchangeCffex = "CFFEX";
const std::string ExchangeSHFE = "SHFE";
const std::string ExchangeDCE = "DCE";
const std::string ExchangeZCE = "ZCE";
const std::string ExchangeINE = "INE";




enum class UserSessionStatus
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
    long Timestamp; //标记这个用户， 区别不同登录时间用户重用 UserSessionID
};



///  行情的最小存储内存结构     总表为hash表， 子结构存储合约级别 MarketData
///  
struct MarketData
{
    std::bitset<1024> subscribers;    // 1024为最大同时在线数量，  后续可修改为配置的方式
    //long SubsTimestamp[1024];          // 解决登出时订阅过期的问题
    CThostFtdcDepthMarketDataField data;
    MarketData(const std::string& instrumentID, const std::string& exchangeID): subscribers(), data()
    {
        std::strcpy(this->data.InstrumentID, instrumentID.c_str());
        std::strcpy(this->data.ExchangeID, exchangeID.c_str());
    }
};




#endif

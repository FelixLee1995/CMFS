/**
* File: CMarketPlugin.h
* Desc: 行情处理插件
* Created by felix on 2021/2/9
*/


#ifndef MARKET_PROCESS_PLUGIN_H
#define MARKET_PROCESS_PLUGIN_H
#pragma once

#include <map>
#include "interface/IPlugin.h"
#include "ds/bizDataStruct.h"
#include "plugin/CUserManagePlugin.h"
#include "plugin/CMarketDataManager.h"
#include "plugin/CUserSessionManager.h"





class CMarketPlugin: public IPlugin
{
private:
    CMarketDataManager::Sptr m_MarketDataManager;
    CUserSessionManager::Ptr m_UserSessionManager;
    CTcpServer::Sptr m_TcpServer;
public:
    CMarketPlugin();
    void Init() override;
    void MsgHandler(const Msg &msg) override;
    void HandleSub(const Msg &msg);
    void HandleUnsub(const Msg &msg);
    void HandleMarketDataRtn(const Msg &msg);

    size_t SubMarketByOneRule(SessionIdType sessionID, int16_t index, const std::string& rule, std::set<CThostFtdcDepthMarketDataField, MarketDataCmp>& dataSet);

    size_t UnSubMarketByOneRule(SessionIdType sessionID, int16_t index, const std::string& rule);

};


#endif
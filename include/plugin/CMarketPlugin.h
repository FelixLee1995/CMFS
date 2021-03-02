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

class CMarketDataManager
{

    private:
        std::map<std::string, MarketData> m_MarketDataMap;


        size_t LoadInstrumentFromCSV(const std::string& filePath = "./config/instruments.csv");


    public:
        using Sptr = std::shared_ptr<CMarketDataManager>;
        CMarketDataManager();


        ~CMarketDataManager();
        

};






class CMarketPlugin: public IPlugin
{
private:
    CMarketDataManager::Sptr m_MarketDataManager;

public:
    CMarketPlugin();
    void Init() override;
    void MsgHandler(const Msg &msg) override;
    void HandleSub(const Msg &msg);
    void HandleUnsub(const Msg &msg);
};


#endif
/**
*  File: CMarketDataManager.h
*  Desc: 行情数据管理器
*  Created by felix on 2021/03/02
*/

#ifndef _MARKET_DATA_MANAGER_H_
#define _MARKET_DATA_MANAGER_H_
#pragma once


#include "core/baseheader.h"
#include <string>
#include <map>
#include <set>


class MarketDataCmp
{   
    public: 
    bool operator()(const CMarketDataExtField& left, const CMarketDataExtField& right) const
    {
        return left.InstrumentID < right.InstrumentID;
    }

};



class CMarketDataManager
{

    private:
        std::map<std::string, MarketData> m_MarketDataMap;

        size_t LoadInstrumentFromCSV(const std::string& filePath = "./config/instruments.csv");

        

    public:
        using Sptr = std::shared_ptr<CMarketDataManager>;
        CMarketDataManager();
        ~CMarketDataManager();
        void GetAllValidMarketData(std::set<CMarketDataExtField, MarketDataCmp>& dataSet);        
        int SubscribeByInstrumentID(int16_t index, const std::string& instrumentID, std::set<CMarketDataExtField, MarketDataCmp> &dataSet);
        int SubscribeByRule(int16_t index, const std::string& rule, std::set<CMarketDataExtField, MarketDataCmp> &dataSet);

        int UnSubscribeByInstrumentID(int16_t index, const std::string &instrumentID);
        int UnSubscribeByRule(int16_t index, const std::string &rule);
        int UnSubscribeAll(int16_t index);

        // 用新分发的行情更新内存数据， 若更新， 则返回true， 若为新增合约， 则返回false
        bool UpdateMarketData(const CMarketDataExtField &);

        void GetMarketDataSubsribers(const std::string &instrumentID, std::bitset<MAX_ONLINE_USERS> &);

        void LockedIterFunc(std::function<void(const MarketData&)>);
};


#endif
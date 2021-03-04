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
    bool operator()(const CThostFtdcDepthMarketDataField& left, const CThostFtdcDepthMarketDataField& right) const
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
        void GetAllValidMarketData(std::set<CThostFtdcDepthMarketDataField, MarketDataCmp>& dataSet);        
        int SubscribeByInstrumentID(int16_t index, const std::string& instrumentID, std::set<CThostFtdcDepthMarketDataField, MarketDataCmp> &dataSet);
        int SubscribeByRule(int16_t index, const std::string& rule, std::set<CThostFtdcDepthMarketDataField, MarketDataCmp> &dataSet);
};


#endif
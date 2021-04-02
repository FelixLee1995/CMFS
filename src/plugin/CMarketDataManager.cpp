#include "plugin/CMarketDataManager.h"
#include <regex>
#include <fstream>

CMarketDataManager::CMarketDataManager()
{
    auto load_cnt = LoadInstrumentFromCSV();
    SPDLOG_INFO("==========  load {} instruments from file", load_cnt);
}

CMarketDataManager::~CMarketDataManager()
{
    
}



size_t CMarketDataManager::LoadInstrumentFromCSV(const std::string& filePath)
{
    size_t instr_count = 0;


    std::ifstream ifs(filePath);
    if (!ifs)
    {
        SPDLOG_ERROR("Failed to find csv file in path: {}", filePath);
        return 0;
    }

    std::string instrumentID;
    char MarketType;
    std::string expireDate;
    std::string exchangeID;

    while (ifs >> instrumentID >> MarketType >> expireDate >> exchangeID)
    {
        SPDLOG_INFO("load instrument: id {}, marketType {}, expireDate {}, exchangeID {}", 
            instrumentID, MarketType, expireDate, exchangeID);
            

        CMarketDataExtField data;
        strcpy(data.ExchangeID, exchangeID.c_str());
        strcpy(data.InstrumentID, instrumentID.c_str());
        data.UpdateMillisec = 100;
        strcpy(data.UpdateTime, "10:12:12");
        data.AskPrice1 = 100;
        data.AskVolume1 = 12;
        data.LastPrice = 123;
        data.Volume = 10;
        data.OpenPrice = 121;
        data.PreClosePrice = 123;
        data.UpperLimitPrice = 150;
        data.LowerLimitPrice = 100;
        data.HighestPrice = 130;
        data.LowestPrice = 115;
        data.ClosePrice = 134;
        data.SettlementPrice = 113;
        data.Turnover = 140;

        m_MarketDataMap.try_emplace(instrumentID, data);
        instr_count++;
    }

    return instr_count;
}

void CMarketDataManager::GetAllValidMarketData(std::set<CMarketDataExtField, MarketDataCmp> &dataSet) 
{
    for (auto& data: m_MarketDataMap)
    {
        dataSet.insert(data.second.Data);
    }
}

int CMarketDataManager::SubscribeByInstrumentID(int16_t index, const std::string &instrumentID, std::set<CMarketDataExtField, MarketDataCmp> &dataSet)
{
    ///
    auto iter = m_MarketDataMap.find(instrumentID);
    if (iter == m_MarketDataMap.end())
    {
        SPDLOG_ERROR("Failed to find such instrument: {}", instrumentID);
        return -1;
    }

    dataSet.insert(iter->second.Data);
    iter->second.Subscribers.set(index);
    return 1;
}


int CMarketDataManager::UnSubscribeByInstrumentID(int16_t index, const std::string &instrumentID)
{
    ///
    auto iter = m_MarketDataMap.find(instrumentID);
    if (iter == m_MarketDataMap.end())
    {
        SPDLOG_ERROR("Failed to find such instrument: {}", instrumentID);
        return -1;
    }

    /// 将marketItem的订阅列表的标记位置reset为0
    iter->second.Subscribers.reset(index);
    return 1;
}


int CMarketDataManager::SubscribeByRule(
    int16_t index, const std::string &rule, std::set<CMarketDataExtField, MarketDataCmp> &dataSet)
{
    int cnt = 0;
    for (auto &marketData : m_MarketDataMap)
    {
        std::regex e(rule);
        std::smatch sm;
        auto if_fit = std::regex_search(marketData.first, sm, e);
        if (!if_fit)
            continue;
        cnt ++;
        dataSet.insert(marketData.second.Data);
        marketData.second.Subscribers.set(index);
    }
    return cnt;
}

int CMarketDataManager::UnSubscribeByRule(int16_t index, const std::string &rule)
{
    int cnt = 0;
    for (auto &marketData : m_MarketDataMap)
    {
        std::regex e(rule);
        std::smatch sm;
        auto if_fit = std::regex_search(marketData.first, sm, e);
        if (!if_fit)
            continue;
        cnt++;
        /// 将marketItem的订阅列表的标记位置reset为0
        marketData.second.Subscribers.reset(index);
    }
    return cnt;
}


int CMarketDataManager::UnSubscribeAll(int16_t index)
{
    int cnt = 0;
    for (auto &marketData : m_MarketDataMap)
    {
        cnt++;
        marketData.second.Subscribers.reset(index);
    }
    return cnt;
}

void CMarketDataManager::GetMarketDataSubsribers(const std::string &instrumentID, std::bitset<MAX_ONLINE_USERS> & subscribers)
{
    auto iter = m_MarketDataMap.find(instrumentID);
    if (iter == m_MarketDataMap.end())
    {
        SPDLOG_ERROR("Failed to find such instrument: {}", instrumentID);
        return;
    }
    subscribers = iter->second.Subscribers;
}
bool CMarketDataManager::UpdateMarketData(const CMarketDataExtField & marketDataField)
{
    auto iter = m_MarketDataMap.find(marketDataField.InstrumentID);
    if (iter == m_MarketDataMap.end())
    {
        SPDLOG_ERROR("Failed to find such instrument: {},  add this instrument to market map", marketDataField.InstrumentID);

        m_MarketDataMap.try_emplace(marketDataField.InstrumentID, marketDataField);

        return false;
    }

    iter->second.Data = marketDataField;


    return true;
}

bool CMarketDataManager::UpdateMarketData(
    const std::string &instrumentid, std::function<void(CMarketDataExtField &market)> updateFunc)
{
    auto iter = m_MarketDataMap.find(instrumentid);
    if (iter == m_MarketDataMap.end())
    {
        SPDLOG_ERROR(
            "Failed to find such instrument: {},  add this instrument to market map", instrumentid);

        CMarketDataExtField newMarketData;
        updateFunc(newMarketData);
        m_MarketDataMap.try_emplace(instrumentid, newMarketData);

        return false;
    }

    updateFunc(iter->second.Data);

    return true;
}

void CMarketDataManager::LockedIterFunc(std::function<void(const MarketData &)> f)
{
    for (auto market : m_MarketDataMap)
    {
        f(market.second);
    }
}

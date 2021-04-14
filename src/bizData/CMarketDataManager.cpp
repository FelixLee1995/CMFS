#include "bizData/CMarketDataManager.h"
#include <regex>
#include <fstream>
#include "core/plantformtools.h"

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
    
    while (!ifs.eof())
    {
        getline(ifs, instrumentID);

        if(instrumentID.empty())
        {
            continue;
        }

        SPDLOG_INFO("load instrument: id {}", 
            instrumentID);
            

        CMarketDataExtField data;
        strcpy(data.InstrumentID, instrumentID.c_str());
        data.UpdateMillisec = 0;
        strcpy(data.UpdateTime, "00:00:00");
        data.AskPrice1 = 0;
        data.AskVolume1 = 0;
        data.LastPrice = 0;
        data.Volume = 0;
        data.OpenPrice = __DBL_MAX__;
        data.PreClosePrice = __DBL_MAX__;
        data.UpperLimitPrice = __DBL_MAX__;
        data.LowerLimitPrice = __DBL_MAX__;
        data.HighestPrice = __DBL_MAX__;
        data.LowestPrice = __DBL_MAX__;
        data.ClosePrice = __DBL_MAX__;
        data.SettlementPrice = __DBL_MAX__;
        data.Turnover = 0;
        
        {
            RWMutex::WriteLock guard(m_Mutex);
            m_MarketDataMap.try_emplace(instrumentID, data);
        }
        instr_count++;
    }

    return instr_count;
}

void CMarketDataManager::GetAllValidMarketData(std::set<CMarketDataExtField, MarketDataCmp> &dataSet) 
{
    RWMutex::ReadLock guard(m_Mutex);
    for (auto& data: m_MarketDataMap)
    {
        dataSet.insert(data.second.Data);
    }
}

int CMarketDataManager::SubscribeByInstrumentID(int16_t index, const std::string &instrumentID, std::set<CMarketDataExtField, MarketDataCmp> &dataSet)
{
    RWMutex::ReadLock guard(m_Mutex);

    auto iter = m_MarketDataMap.find(instrumentID);
    if (iter == m_MarketDataMap.end())
    {
        SPDLOG_ERROR("Failed to find such instrument: {}", instrumentID);
        return COMMON_FAILURE;
    }

    dataSet.insert(iter->second.Data);
    iter->second.Subscribers.set(index);
    return COMMON_OK;
}


int CMarketDataManager::UnSubscribeByInstrumentID(int16_t index, const std::string &instrumentID)
{
    RWMutex::ReadLock guard(m_Mutex);

    auto iter = m_MarketDataMap.find(instrumentID);
    if (iter == m_MarketDataMap.end())
    {
        SPDLOG_ERROR("Failed to find such instrument: {}", instrumentID);
        return COMMON_FAILURE;
    }

    /// 将marketItem的订阅列表的标记位置reset为0
    iter->second.Subscribers.reset(index);
    return COMMON_OK;
}


int CMarketDataManager::SubscribeByRule(
    int16_t index, const std::string &rule, std::set<CMarketDataExtField, MarketDataCmp> &dataSet)
{
    int cnt = 0;
    
    RWMutex::ReadLock guard(m_Mutex);
    
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

    RWMutex::ReadLock guard(m_Mutex);

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
    RWMutex::ReadLock guard(m_Mutex);
    for (auto &marketData : m_MarketDataMap)
    {
        cnt++;
        marketData.second.Subscribers.reset(index);
    }
    return cnt;
}

void CMarketDataManager::GetMarketDataSubsribers(const std::string &instrumentID, std::bitset<MAX_ONLINE_USERS> & subscribers)
{
    RWMutex::ReadLock guard(m_Mutex);

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

    RWMutex::WriteLock guard(m_Mutex);

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

    RWMutex::WriteLock guard(m_Mutex);

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
    RWMutex::ReadLock guard(m_Mutex);

    for (auto market : m_MarketDataMap)
    {
        f(market.second);
    }
}

//
// Created by felix on 2021/2/9.
//

#include "plugin/CMarketPlugin.h"

CMarketPlugin::CMarketPlugin(): IPlugin("MarketPlugin")
{}

void CMarketPlugin::Init()
{


    m_MarketDataManager.reset(Singleton<CMarketDataManager>::GetInstance());



    Subscribe(TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_SUB);
    Subscribe(TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_UNSUB);



    
}
void CMarketPlugin::MsgHandler(const Msg &msg)
{
    switch(msg.Header.FuncId) {
        case FUNC_REQ_MARKET_SUB: {
            HandleSub(msg);
            break;
        }
        case FUNC_REQ_MARKET_UNSUB: {
            HandleUnsub(msg);
            break;
        }
        default: {
            SPDLOG_ERROR("NO HANDLER FOR FUNCID: {}", msg.Header.FuncId);
            return;
        }
    }
}
void CMarketPlugin::HandleSub(const Msg &msg) {}
void CMarketPlugin::HandleUnsub(const Msg &msg) {}




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
            
        m_MarketDataMap.try_emplace(instrumentID, instrumentID, exchangeID);
        instr_count++;
    }

    return instr_count;
}

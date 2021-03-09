//
// Created by felix on 2021/2/9.
//

#include "plugin/CMarketPlugin.h"
#include "api/ctp_ext/ctp_ftdc_proto.h"

using namespace ctp_ftd;

CMarketPlugin::CMarketPlugin(): IPlugin("MarketPlugin")
{}

void CMarketPlugin::Init()
{
    spdlog::set_level(spdlog::level::debug);

    m_UserSessionManager.reset(Singleton<CUserSessionManager>::GetInstance());
    m_MarketDataManager.reset(Singleton<CMarketDataManager>::GetInstance());
    m_TcpServer.reset(Singleton<CTcpServer>::GetInstance());


    Subscribe(TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_SUB);
    Subscribe(TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_UNSUB);
    Subscribe(TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_SNAPSHOT_RTN);
    
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
        case FUNC_REQ_MARKET_SNAPSHOT_RTN:
        {
            HandleMarketDataRtn(msg);
            break;
        }
        default: {
            SPDLOG_ERROR("NO HANDLER FOR FUNCID: {}", msg.Header.FuncId);
            return;
        }
    }
}
void CMarketPlugin::HandleSub(const Msg &msg)
{
    /// 1. parse req field, 得到订阅参数
    auto sessionId = msg.Header.SessionId;
    auto cnt = msg.Header.Count;


    std::set<CThostFtdcDepthMarketDataField, MarketDataCmp> marketDataSnapshotSet;


    /// 2. 根据订阅参数分别进行订阅
    int error_id = 0;
    std::string error_msg = "Subscribe success!";

    auto index = m_UserSessionManager->CheckIfLogin(sessionId);
    if (index>= 0)
    {


    std::vector<std::string> instrsVec;
    uint8_t i = 0;    

    while (i < cnt)
    {
        auto field = (CThostFtdcSpecificInstrumentField *)  (msg.Pack + sizeof(ftdc_field_header) + i * (sizeof(ftdc_field_header) +  sizeof(CThostFtdcSpecificInstrumentField)));
        instrsVec.emplace_back(field->InstrumentID);
        i++;
        if (strcmp(field->InstrumentID, "*") == 0)
        {
            break;
        }
    }


        for (auto instr : instrsVec)
        {
            SubMarketByOneRule(sessionId, index, instr, marketDataSnapshotSet);
        }

        if (marketDataSnapshotSet.empty())
        {
            error_id = 10000;
            error_msg = "Subscribe failed, no such instrument";
        }
    }
    else
    {
        error_id = 6;
        error_msg = "User not login yet";
    }

  
    /// 3. 发送订阅应答
    /// 订阅成功， 发送所有行情订阅应答

    CThostFtdcSpecificInstrumentField specField = {0};


    if (error_id == 0)
    {
        for (auto marketData : marketDataSnapshotSet) 
        {
            memcpy(specField.InstrumentID, marketData.InstrumentID, sizeof(specField.InstrumentID));
            TCP_SEND_RSPINFO(TOPIC_USER_MANAGE, m_TcpServer, sessionId, ftdc_tid_RspSubMarketData, CThostFtdcSpecificInstrumentField, &specField, 
                ftdc_fid_SpecificInstrumentField, error_id, error_msg.c_str());
        }
    }
    /// 订阅失败， 只发送一条
    else
    {
        TCP_SEND_RSPINFO(TOPIC_USER_MANAGE, m_TcpServer, sessionId, ftdc_tid_RspSubMarketData, CThostFtdcSpecificInstrumentField, &specField, 
                ftdc_fid_SpecificInstrumentField, error_id, error_msg.c_str());
    }


    /// 4. 发送行情快照
    for (auto marketData : marketDataSnapshotSet) 
        {

            if (marketData.OpenPrice == 0)
            {

                continue;
            }
            memcpy(specField.InstrumentID, marketData.InstrumentID, sizeof(specField.InstrumentID));
            TCP_SEND_RTNINFO(TOPIC_USER_MANAGE, m_TcpServer, sessionId, ftdc_tid_RtnDepthMarketData_snap, CThostFtdcDepthMarketDataField, &marketData, 
                ftdc_tid_RtnDepthMarketData_snap);
        }


}



size_t CMarketPlugin::SubMarketByOneRule(SessionIdType sessionid, int16_t index, const std::string& rule, std::set<CThostFtdcDepthMarketDataField, MarketDataCmp>& dataSet)
{
    size_t subs_cnt = 0;
    auto wildcard_index = rule.find('*');
    if (rule == "*")
    {
        /// 全部订阅
        m_UserSessionManager->SubscribeAllBySessionID(sessionid);
        m_MarketDataManager->GetAllValidMarketData(dataSet);

    }
    else if( wildcard_index != rule.npos)
    {
        /// 模糊匹配
        /// 考虑使用正则进行匹配
        m_MarketDataManager->SubscribeByRule(index, rule.substr(0, wildcard_index), dataSet);
    }
    else
    {
        /// 使用合约号进行订阅
        m_MarketDataManager->SubscribeByInstrumentID(index, rule, dataSet);
    }

    return subs_cnt;
}


void CMarketPlugin::HandleUnsub(const Msg &msg) {}


void CMarketPlugin::HandleMarketDataRtn(const Msg &msg)
{

    auto* marketData =  (CThostFtdcDepthMarketDataField *)  (msg.Pack + sizeof(ftdc_field_header));
    //auto sessionId = msg.Header.SessionId;


    m_MarketDataManager->UpdateMarketData(*marketData);

    std::bitset<MAX_ONLINE_USERS> subscribers;
    std::set<SessionIdType> sessionIdSet;

    m_MarketDataManager->GetMarketDataSubsribers(marketData->InstrumentID, subscribers);


    m_UserSessionManager->CheckIfSubs(subscribers, sessionIdSet);

    for (auto id:sessionIdSet)
    {
        TCP_SEND_RTNINFO(TOPIC_MARKET_PROCESS, m_TcpServer, id, ftdc_tid_RtnDepthMarketData_snap, CThostFtdcDepthMarketDataField, marketData,
                         ftdc_tid_RtnDepthMarketData_snap);
    }



}



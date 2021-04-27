//
// Created by felix on 2021/2/9.
//

#include "plugin/CMarketPlugin.h"
#include "api/ctp_ext/ctp_ftdc_proto.h"
#include "core/marco.h"

using namespace ctp_ftd;

CMarketPlugin::CMarketPlugin() : IPlugin("MarketPlugin") {}

void CMarketPlugin::Init()
{
    m_UserSessionManager.reset(Singleton<CUserSessionManager>::GetInstance());
    m_MarketDataManager.reset(Singleton<CMarketDataManager>::GetInstance());
    m_TcpServer.reset(Singleton<CTcpServer>::GetInstance());

    Subscribe(TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_SUB);
    Subscribe(TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_UNSUB);
    Subscribe(TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_SNAPSHOT_RTN);
    Subscribe(TOPIC_USER_MANAGE, FUNC_REQ_USER_TIMEOUT);
}
void CMarketPlugin::MsgHandler(const Msg &msg)
{
    switch (msg.Header.FuncId)
    {
        case FUNC_REQ_MARKET_SUB:
        {
            HandleSub(msg);
            break;
        }
        case FUNC_REQ_MARKET_UNSUB:
        {
            HandleUnsub(msg);
            break;
        }
        case FUNC_REQ_MARKET_SNAPSHOT_RTN:
        {
            HandleMarketDataRtn(msg);
            break;
        }
        default:
        {
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

    SPDLOG_INFO("HandleSub, sessionid is {}", sessionId);

    std::set<CMarketDataExtField, MarketDataCmp> marketDataSnapshotSet;

    /// 2. 根据订阅参数分别进行订阅
    int error_id = 0;
    std::string error_msg = "Subscribe success!";

    auto index = m_UserSessionManager->CheckIfLogin(sessionId);

    std::vector<std::string> instrsVec;

    if (MY_LIKELY(index >= 0))
    {
        uint8_t i = 0;

        while (i < cnt)
        {
            auto field = (CThostFtdcSpecificInstrumentField *)(msg.Pack + sizeof(ftdc_field_header) +
                i * (sizeof(ftdc_field_header) + sizeof(CThostFtdcSpecificInstrumentField)));
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
            error_id = ErrorID_NoSuchInstr;
            error_msg = ErrorMsg_NoSuchInstr;
        }
    }
    else
    {
        error_id = ErrorID_NotLogin;
        error_msg = ErrorMsg_NotLogin;
    }

    /// 3. 发送订阅应答
    /// 订阅成功， 发送所有行情订阅应答

    CThostFtdcSpecificInstrumentField specField = {0};

    if (error_id == 0)
    {
        for (auto instr : instrsVec)
        {
            strcpy(specField.InstrumentID, instr.c_str());
            m_TcpServer->SendRspFtdc(TOPIC_USER_MANAGE, sessionId, ftdc_tid_RspSubMarketData,
                reinterpret_cast<const char *>(&specField), sizeof(CThostFtdcSpecificInstrumentField),
                ftdc_fid_SpecificInstrumentField, error_id, error_msg.c_str());
        }
    }
    /// 订阅失败， 只发送一条
    else
    {
        m_TcpServer->SendRspFtdc(TOPIC_USER_MANAGE, sessionId, ftdc_tid_RspSubMarketData,
            reinterpret_cast<const char *>(&specField), sizeof(CThostFtdcSpecificInstrumentField),
            ftdc_fid_SpecificInstrumentField, error_id, error_msg.c_str());
    }


    /// 4. 发送行情快照，  以十条快照为一个ftd包组装发送
    std::vector<CMarketDataExtField> dataVec;
    dataVec.reserve(10);
    for (auto iter = marketDataSnapshotSet.begin(); iter != marketDataSnapshotSet.end(); iter++)
    {
        /// _DBL_MAX_ 为初始化数据，  此时表示从未有快照更新， 无需发送
        if (MY_LIKELY(iter->OpenPrice != __DBL_MAX__))
        {
            dataVec.emplace_back(*iter);
        }

        if (dataVec.size() >= 10)
        {
            m_TcpServer->SendMultiRtnFtdc(TOPIC_MARKET_PROCESS, sessionId, ftdc_tid_RtnDepthMarketData_snap,
                reinterpret_cast<const char *>(dataVec.data()), sizeof(CMarketDataExtField), ftdc_fid_DepthMarketDataField,
                dataVec.size());
            dataVec.clear();

            CommonSleep(1);    ///  TODO   是否有更好的应对方式，  解决发送快照过快导致的api断线问题
        }
    }


    /// 不足10条的部分， 最后发送出去
    if (dataVec.size() > 0)
    {
        //auto cnt = dataVec.size();
        m_TcpServer->SendMultiRtnFtdc(TOPIC_MARKET_PROCESS, sessionId, ftdc_tid_RtnDepthMarketData_snap,
            reinterpret_cast<const char *>(dataVec.data()), sizeof(CMarketDataExtField), ftdc_fid_DepthMarketDataField,
            dataVec.size());
    }
}

size_t CMarketPlugin::SubMarketByOneRule(
    SessionIdType sessionid, int16_t index, const std::string &rule, std::set<CMarketDataExtField, MarketDataCmp> &dataSet)
{
    size_t subs_cnt = 0;
    auto wildcard_index = rule.find('*');


    ///  若通配符订阅，  检查该客户的权限
    if (wildcard_index != rule.npos)
    {
        auto if_wildcard = m_UserSessionManager->CheckIfWildcard(sessionid);
        if (if_wildcard == false)
            SPDLOG_ERROR("sessionid {} has no wildcard permission", sessionid);
            return subs_cnt;
    }


    if (rule == "*")
    {
        /// 全部订阅
        m_UserSessionManager->SubscribeAllBySessionID(sessionid);
        m_MarketDataManager->GetAllValidMarketData(dataSet);
    }
    else if (wildcard_index != rule.npos)
    {
        /// 模糊匹配
        /// 使用正则进行匹配  效率一般
        m_MarketDataManager->SubscribeByRule(index, rule.substr(0, wildcard_index), dataSet);
    }
    else
    {
        /// 使用合约号进行订阅
        m_MarketDataManager->SubscribeByInstrumentID(index, rule, dataSet);
    }

    return subs_cnt;
}

size_t CMarketPlugin::UnSubMarketByOneRule(SessionIdType sessionid, int16_t index, const std::string &rule)
{
    size_t subs_cnt = 0;
    auto wildcard_index = rule.find('*');
    if (rule == "*")
    {
        /// 全部取消订阅
        m_UserSessionManager->UnSubscribeAllBySessionID(sessionid);
        m_MarketDataManager->UnSubscribeAll(index);
    }
    else if (wildcard_index != rule.npos)
    {
        /// 模糊匹配取消订阅
        /// 使用正则进行匹配
        m_MarketDataManager->UnSubscribeByRule(index, rule.substr(0, wildcard_index));
    }
    else
    {
        /// 使用合约号进行取消订阅
        m_MarketDataManager->UnSubscribeByInstrumentID(index, rule);
    }

    return subs_cnt;
}

void CMarketPlugin::HandleUnsub(const Msg &msg)
{
    /// 1. parse req field, 得到订阅参数
    auto sessionId = msg.Header.SessionId;
    auto cnt = msg.Header.Count;

    /// 2. 根据订阅参数分别进行订阅
    int error_id = 0;
    std::string error_msg = "UnSubscribe success!";

    auto index = m_UserSessionManager->CheckIfLogin(sessionId);

    std::vector<std::string> instrsVec;

    if (MY_LIKELY(index >= 0))
    {
        uint8_t i = 0;

        while (i < cnt)
        {
            auto field = (CThostFtdcSpecificInstrumentField *)(msg.Pack + sizeof(ftdc_field_header) +
                i * (sizeof(ftdc_field_header) + sizeof(CThostFtdcSpecificInstrumentField)));
            instrsVec.emplace_back(field->InstrumentID);
            i++;
            if (strcmp(field->InstrumentID, "*") == 0)
            {
                break;
            }
        }

        for (auto instr : instrsVec)
        {
            UnSubMarketByOneRule(sessionId, index, instr);
        }
    }
    else
    {
        error_id = ErrorID_NotLogin;  
        error_msg = ErrorMsg_NotLogin;
    }

    /// 3. 发送订阅应答
    /// 取消订阅成功， 发送行情取消订阅应答

    CThostFtdcSpecificInstrumentField specField = {0};

    if (MY_LIKELY(error_id == 0))
    {
        for (auto instr : instrsVec)
        {
            strcpy(specField.InstrumentID, instr.c_str());
            m_TcpServer->SendRspFtdc(TOPIC_USER_MANAGE, sessionId, ftdc_tid_RspUnSubMarketData,
                reinterpret_cast<const char *>(&specField), sizeof(CThostFtdcSpecificInstrumentField),
                ftdc_fid_SpecificInstrumentField, error_id, error_msg.c_str());
        }
    }
    /// 订阅失败， 只发送一条
    else
    {
        m_TcpServer->SendRspFtdc(TOPIC_USER_MANAGE, sessionId, ftdc_tid_RspUnSubMarketData,
            reinterpret_cast<const char *>(&specField), sizeof(CThostFtdcSpecificInstrumentField),
            ftdc_fid_SpecificInstrumentField, error_id, error_msg.c_str());
    }
}

void CMarketPlugin::HandleMarketDataRtn(const Msg &msg)
{
    auto marketData = (CMarketDataExtField *)msg.Pack;

    std::bitset<MAX_ONLINE_USERS> subscribers;
    std::set<SessionIdType> sessionIdSet;

    m_MarketDataManager->GetMarketDataSubsribers(marketData->InstrumentID, subscribers);

    m_UserSessionManager->CheckIfSubs(subscribers, sessionIdSet);

    for (auto id : sessionIdSet)
    {
        m_TcpServer->SendRtnFtdc(TOPIC_MARKET_PROCESS, id, ftdc_tid_RtnDepthMarketData_snap,
            reinterpret_cast<const char *>(marketData), sizeof(CMarketDataExtField), ftdc_fid_DepthMarketDataField);
    }
}

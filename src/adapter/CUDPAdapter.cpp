#include "adapter/CUDPAdapter.h"
#include "utils/Utils.h"

CUdpMarketAdapter::CUdpMarketAdapter(const nlohmann::json &config)
    : IMarketRecvAdapter("UdpMarketAdapter"), api(nullptr), m_WorkFlag(false), m_GuardThread(nullptr)
{
    api = new MyUdpApi(config);

}

CUdpMarketAdapter::~CUdpMarketAdapter()
{
    api->Release();
    delete api;
}


void CUdpMarketAdapter::Init()
{   
    SPDLOG_INFO("start UdpMarketAdapter");
    m_WorkFlag.store(true);
    m_GuardThread = std::make_shared<std::thread>(&CUdpMarketAdapter::GuardFunc, this);
}

void CUdpMarketAdapter::GuardFunc()
{   

    while (m_WorkFlag)
    {
        if (!api)
        {
            SPDLOG_ERROR("api is null");
            break;
        }

        if (api->m_ConnectStatus.load() == false)
        {
     
            //TODO  判断 api 重连时是否复用
            SPDLOG_INFO("try connecting");
            api->Init();
        }

        CommonSleep(1000*30);
    }

}


void CUdpMarketAdapter::Stop()
{   
    m_WorkFlag.store(false);
    
    if (m_GuardThread->joinable())
    {
        m_GuardThread->join();
    }

    m_GuardThread.reset();
}



MyUdpApi::MyUdpApi(const nlohmann::json &config): m_IsConnecting(false), m_ConnectStatus(false)
{
    m_FlowManager.reset(Singleton<CFlowManager>::GetInstance());
    m_MarketDataManager.reset(Singleton<CMarketDataManager>::GetInstance());

    m_Path = config["path"].get<std::string>();
    m_Username = config["username"].get<std::string>();
    m_Pwd = config["pwd"].get<std::string>();
}

void MyUdpApi::Init()
{
    pApi = GtjaMdUserApi::CreateMdUserApi("api.log");
	pApi->RegisterSpi(this);
	pApi->RegisterFront(m_Path.c_str());
    //pApi->SetConfig("efvi_nic", "enp23s0f0");
	pApi->Init();
	//pApi->Join();

}

    void MyUdpApi::Release()
    {
        pApi->RegisterSpi(nullptr);
        pApi->Join();
        pApi->Release();
    }

void MyUdpApi::OnFrontConnected()
{
    SPDLOG_INFO("OnFrontConnected");
    GtjaMdReqUserLoginField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.UserID, m_Username.c_str());
    strcpy(req.Password, m_Pwd.c_str());
    pApi->ReqUserLogin(&req, 0);
}

void MyUdpApi::OnFrontDisconnected(int nReason)
{
    m_ConnectStatus.store(false);
}

///登录请求响应
void MyUdpApi::OnRspUserLogin(
    const GtjaMdRspUserLoginField *pRspUserLogin, const GtjaMdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID == 0)
    {
        m_ConnectStatus.store(true);     
    }
    else if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        m_ConnectStatus.store(false);
    }
}

void MyUdpApi::OnRtnDepthSnapshot(const GtjaMdV3::GtjaMdInstrumentFieldV3 *pInstrument,
    const GtjaMdV3::TGtjaDateTime *pStamp, const GtjaMdV3::GtjaMdTradeInfoFieldV3 *pTradeInfo,
    const GtjaMdV3::GtjaMdBaseInfoFieldV3 *pBaseInfo, const GtjaMdV3::GtjaMdStaticInfoFieldV3 *pStaticInfo, int MBLLength,
    const GtjaMdV3::GtjaMdMBLFieldV3 *pMBL)
{
    //    if (pMBL && MBLLength > 0)
    //    {
    //        std::cout << "\t" << pMBL[0].BidPrice << "\t" << pMBL[0].BidVolume << "\t" << pMBL[0].AskPrice << "\t"
    //                  << pMBL[0].AskVolume;
    //    }

    SPDLOG_INFO("Rtn marketdata, {}, vol {}", pInstrument->InstrumentID, pTradeInfo->Volume);

    CMarketDataExtField marketDataOut;

    m_MarketDataManager->UpdateMarketData(pInstrument->InstrumentID, [&](CMarketDataExtField &marketData) {
        strcpy(marketData.InstrumentID, pInstrument->InstrumentID);
        strcpy(marketData.ExchangeID, ConvertExchange(pInstrument->ExchangeType).c_str());

        auto updateTime = fmt::format("{:02d}:{:02d}:{:02d}", pStamp->Time.Hour, pStamp->Time.Minite, pStamp->Time.Seccond);

        strcpy(marketData.UpdateTime, updateTime.c_str());
        marketData.UpdateMillisec = htonl(pStamp->Time.MilliSec);

        if (pTradeInfo)
        {
            marketData.LastPrice = MY_HTONF(pTradeInfo->LastPrice);
            marketData.Turnover = MY_HTONF(pTradeInfo->Turnover);
            marketData.OpenInterest = MY_HTONF(pTradeInfo->OpenInterest);
            marketData.Volume = htonl((int)pTradeInfo->Volume);
        }

        if (pBaseInfo)
        {
            strcpy(marketData.TradingDay, pBaseInfo->TradingDay);
            marketData.PreSettlementPrice = MY_HTONF(pBaseInfo->PreSettlementPrice);
            marketData.PreClosePrice = MY_HTONF(pBaseInfo->PreClosePrice);
            marketData.PreOpenInterest = MY_HTONF(pBaseInfo->PreOpenInterest);
            marketData.UpperLimitPrice = MY_HTONF(pBaseInfo->UpperLimitPrice);
            marketData.LowerLimitPrice = MY_HTONF(pBaseInfo->LowerLimitPrice);
        }

        if (pStaticInfo)
        {
            std::cout << "highestPrice " << pStaticInfo->HighestPrice << std::endl;
             marketData.OpenPrice = MY_HTONF(pStaticInfo->OpenPrice);
            marketData.ClosePrice = MY_HTONF(pStaticInfo->ClosePrice);
            marketData.SettlementPrice = MY_HTONF(pStaticInfo->SettlementPrice);
            marketData.HighestPrice = MY_HTONF(pStaticInfo->HighestPrice);
            marketData.LowestPrice = MY_HTONF(pStaticInfo->LowestPrice);
        }

        int depth = 1;
        auto st = const_cast<GtjaMdV3::GtjaMdMBLFieldV3 *>(pMBL);
        while (depth <= MBLLength)
        {
            switch (depth)
            {
                case 1:
                {
                    marketData.AskPrice1 = MY_HTONF(st->AskPrice);
                    marketData.AskVolume1 = htonl(st->AskVolume);
                    marketData.BidPrice1 = MY_HTONF(st->BidPrice);
                    marketData.BidVolume1 = htonl(st->BidVolume);
                    break;
                }
                case 2:
                {
                    marketData.AskPrice2 = MY_HTONF(st->AskPrice);
                    marketData.AskVolume2 = htonl(st->AskVolume);
                    marketData.BidPrice2 = MY_HTONF(st->BidPrice);
                    marketData.BidVolume2 = htonl(st->BidVolume);
                    break;
                }
                case 3:
                {
                    marketData.AskPrice3 = MY_HTONF(st->AskPrice);
                    marketData.AskVolume3 = htonl(st->AskVolume);
                    marketData.BidPrice3 = MY_HTONF(st->BidPrice);
                    marketData.BidVolume3 = htonl(st->BidVolume);
                    break;
                }
                case 4:
                {
                    marketData.AskPrice4 = MY_HTONF(st->AskPrice);
                    marketData.AskVolume4 = htonl(st->AskVolume);
                    marketData.BidPrice4 = MY_HTONF(st->BidPrice);
                    marketData.BidVolume4 = htonl(st->BidVolume);
                    break;
                }
                case 5:
                {
                    marketData.AskPrice5 = MY_HTONF(st->AskPrice);
                    marketData.AskVolume5 = htonl(st->AskVolume);
                    marketData.BidPrice5 = MY_HTONF(st->BidPrice);
                    marketData.BidVolume5 = htonl(st->BidVolume);
                    break;
                }
                default:
                    break;
            }
            st++;
            depth++;
        }

        marketDataOut = marketData;
    });

    m_FlowManager->PubBizMsg2Plugin(TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_SNAPSHOT_RTN, 0, reinterpret_cast<const char*>(&marketDataOut),
        sizeof(CMarketDataExtField), 1);
}

void MyUdpApi::OnRspLastSnapshot(const GtjaDepthMarketDataField *pDepthMarketData)
{
    m_MarketDataManager->UpdateMarketData(pDepthMarketData->InstrumentID, [&](CMarketDataExtField &marketData) {
        strcpy(marketData.InstrumentID, pDepthMarketData->InstrumentID);

        // 无交易所字段？？
        //strcpy(marketData.ExchangeID, ConvertExchange(pDepthMarketData->ExchangeID).c_str());

        //auto updateTime = fmt::format("{:02d}:{:02d}:{:02d}", pStamp->Time.Hour, pStamp->Time.Minite, pStamp->Time.Seccond);

        strcpy(marketData.UpdateTime, pDepthMarketData->UpdateTime);
        marketData.UpdateMillisec = htonl(pDepthMarketData->UpdateMillisec);

        marketData.LastPrice = MY_HTONF(pDepthMarketData->LastPrice);
        marketData.Turnover = MY_HTONF(pDepthMarketData->Turnover);
        marketData.OpenInterest = MY_HTONF(pDepthMarketData->OpenInterest);
        marketData.Volume = htonl((int)pDepthMarketData->Volume);

        strcpy(marketData.TradingDay, pDepthMarketData->TradingDay);
        marketData.PreSettlementPrice = MY_HTONF(pDepthMarketData->PreSettlementPrice);
        marketData.PreClosePrice = MY_HTONF(pDepthMarketData->PreClosePrice);
        marketData.PreOpenInterest = MY_HTONF(pDepthMarketData->PreOpenInterest);
        marketData.UpperLimitPrice = MY_HTONF(pDepthMarketData->UpperLimitPrice);
        marketData.LowerLimitPrice = MY_HTONF(pDepthMarketData->LowerLimitPrice);

        marketData.OpenPrice = MY_HTONF(pDepthMarketData->OpenPrice);
        marketData.ClosePrice = MY_HTONF(pDepthMarketData->ClosePrice);
        marketData.SettlementPrice = MY_HTONF(pDepthMarketData->SettlementPrice);

        marketData.AskPrice1 = MY_HTONF(pDepthMarketData->AskPrice1);
        marketData.AskVolume1 = htonl(pDepthMarketData->AskVolume1);
        marketData.BidPrice1 = MY_HTONF(pDepthMarketData->BidPrice1);
        marketData.BidVolume1 = htonl(pDepthMarketData->BidVolume1);

        marketData.AskPrice2 = MY_HTONF(pDepthMarketData->AskPrice2);
        marketData.AskVolume2 = htonl(pDepthMarketData->AskVolume2);
        marketData.BidPrice2 = MY_HTONF(pDepthMarketData->BidPrice2);
        marketData.BidVolume2 = htonl(pDepthMarketData->BidVolume2);

        marketData.AskPrice3 = MY_HTONF(pDepthMarketData->AskPrice3);
        marketData.AskVolume3 = htonl(pDepthMarketData->AskVolume3);
        marketData.BidPrice3 = MY_HTONF(pDepthMarketData->BidPrice3);
        marketData.BidVolume3 = htonl(pDepthMarketData->BidVolume3);

        marketData.AskPrice4 = MY_HTONF(pDepthMarketData->AskPrice3);
        marketData.AskVolume4 = htonl(pDepthMarketData->AskVolume3);
        marketData.BidPrice4 = MY_HTONF(pDepthMarketData->BidPrice3);
        marketData.BidVolume4 = htonl(pDepthMarketData->BidVolume3);

        marketData.AskPrice5 = MY_HTONF(pDepthMarketData->AskPrice3);
        marketData.AskVolume5 = htonl(pDepthMarketData->AskVolume3);
        marketData.BidPrice5 = MY_HTONF(pDepthMarketData->BidPrice3);
        marketData.BidVolume5 = htonl(pDepthMarketData->BidVolume3);
    });
}

std::string MyUdpApi::ConvertExchange(uint8_t exchange)
{
    switch(exchange)
    {
        case TExchangeType::_et_cffex :
            return "CFFEX";
        case TExchangeType::_et_czce :
            return "ZCE";
        case TExchangeType::_et_dce :
            return "DCE";
        case TExchangeType::_et_shfe :
            return "SHFE";
        case TExchangeType::_et_ine :
            return "INE";
        case TExchangeType::_et_sse :
            return "SSE";
        case TExchangeType::_et_szse :
            return "SZSE";
        case TExchangeType::_et_sge :
            return "SGE";
        default:
            return "UNKNOWN";
    }
}

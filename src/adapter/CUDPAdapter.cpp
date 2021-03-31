#include "adapter/CUDPAdapter.h"
#include "utils/Utils.h"



CUdpMarketAdapter::CUdpMarketAdapter(const nlohmann::json &config)
: IMarketRecvAdapter("UdpMarketAdapter")
{
      api = new MyUdpApi(config);
}


CUdpMarketAdapter::~CUdpMarketAdapter()
{

}


void CUdpMarketAdapter::Init()
{   
    SPDLOG_INFO("start UdpMarketAdapter");
    api->Init();
}


void CUdpMarketAdapter::Stop()
{   

}



MyUdpApi::MyUdpApi(const nlohmann::json &config) 
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

void MyUdpApi::OnFrontConnected()
{
    SPDLOG_INFO("OnFrontConnected");
    GtjaMdReqUserLoginField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.UserID, m_Username.c_str());
    strcpy(req.Password, m_Pwd.c_str());
    pApi->ReqUserLogin(&req, 0);
}

///登录请求响应
void MyUdpApi::OnRspUserLogin(
    const GtjaMdRspUserLoginField *pRspUserLogin, const GtjaMdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID)
    {
        printf("RspUserLogin,Error:%d,%s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else if (pRspUserLogin)
    {
        printf("RspUserLogin Succ:LoginTime:%s\n", pRspUserLogin->LoginTime);
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

    CMarketDataExtField marketDataOut;

    m_MarketDataManager->UpdateMarketData(pInstrument->InstrumentID, [&](CMarketDataExtField &marketData) {
        strcpy(marketData.InstrumentID, pInstrument->InstrumentID);
        strcpy(marketData.ExchangeID, ConvertExchange(pInstrument->ExchangeType).c_str());

        auto updateTime = fmt::format("{:02d}:{:02d}:{:02d}", pStamp->Time.Hour, pStamp->Time.Minite, pStamp->Time.Seccond);

        strcpy(marketData.UpdateTime, updateTime.c_str());
        marketData.UpdateMillisec = pStamp->Time.MilliSec;

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
                    marketData.AskPrice1 = st->AskPrice;
                    marketData.AskVolume1 = st->AskVolume;
                    marketData.BidPrice1 = st->BidPrice;
                    marketData.BidVolume1 = st->BidVolume;
                    break;
                }
                case 2:
                {
                    marketData.AskPrice2 = st->AskPrice;
                    marketData.AskVolume2 = st->AskVolume;
                    marketData.BidPrice2 = st->BidPrice;
                    marketData.BidVolume2 = st->BidVolume;
                    break;
                }
                case 3:
                {
                    marketData.AskPrice3 = st->AskPrice;
                    marketData.AskVolume3 = st->AskVolume;
                    marketData.BidPrice3 = st->BidPrice;
                    marketData.BidVolume3 = st->BidVolume;
                    break;
                }
                case 4:
                {
                    marketData.AskPrice4 = st->AskPrice;
                    marketData.AskVolume4 = st->AskVolume;
                    marketData.BidPrice4 = st->BidPrice;
                    marketData.BidVolume4 = st->BidVolume;
                    break;
                }
                case 5:
                {
                    marketData.AskPrice5 = st->AskPrice;
                    marketData.AskVolume5 = st->AskVolume;
                    marketData.BidPrice5 = st->BidPrice;
                    marketData.BidVolume5 = st->BidVolume;
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

    PUB_BIZ_MSG_TO_PLUGIN(m_FlowManager, TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_SNAPSHOT_RTN, 0, &marketDataOut,
        sizeof(CMarketDataExtField), 1);
}

void MyUdpApi::OnRspLastSnapshot(const GtjaDepthMarketDataField *pDepthMarketData)
{
    // std::cout << "OnRspLastSnapshot:" << pDepthMarketData->InstrumentID << "\t" << pDepthMarketData->UpdateTime << "."
    //           << pDepthMarketData->UpdateMillisec;
    // std::cout << "\t" << pDepthMarketData->LastPrice << "\t" << pDepthMarketData->Volume;
    // std::cout << "\t" << pDepthMarketData->BidPrice1 << "\t" << pDepthMarketData->BidVolume1 << "\t"
    //           << pDepthMarketData->AskPrice1 << "\t" << pDepthMarketData->AskVolume1;
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

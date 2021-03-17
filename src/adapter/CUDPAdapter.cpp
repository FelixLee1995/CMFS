#include "adapter/CUDPAdapter.h"




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
    std::cout << "OnFrontConnected ===========================" << std::endl;
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
    // std::cout << "OnRtnDepthMarketData: " << pInstrument->ExchangeType << pInstrument->InstrumentID << "\t"
    //           << std::setfill('0') << std::setw(2) << (int)pStamp->Time.Hour << ":" << (int)pStamp->Time.Minite << ":"
    //           << pStamp->Time.Seccond << "." << std::setw(3) << pStamp->Time.MilliSec;

    if (pMBL && MBLLength > 0)
    {
        std::cout << "\t" << pMBL[0].BidPrice << "\t" << pMBL[0].BidVolume << "\t" << pMBL[0].AskPrice << "\t"
                  << pMBL[0].AskVolume;
    }

    CMarketDataExtField marketData;
    strcpy(marketData.ExchangeID, ConvertExchange(pInstrument->ExchangeType).c_str());
    strcpy(marketData.InstrumentID, pInstrument->InstrumentID);
    
    if (pTradeInfo)
    {
        marketData.LastPrice = pTradeInfo->LastPrice;
        marketData.Turnover = pTradeInfo->Turnover;
        marketData.OpenInterest = pTradeInfo->OpenInterest;
        marketData.Volume = pTradeInfo->Volume;
    }

    if (pBaseInfo)
    {
        strcpy(marketData.TradingDay, pBaseInfo->TradingDay);
        marketData.PreSettlementPrice = pBaseInfo->PreSettlementPrice;
        marketData.PreClosePrice = pBaseInfo->PreClosePrice;
        marketData.PreOpenInterest = pBaseInfo->PreOpenInterest;
        marketData.UpperLimitPrice = pBaseInfo->UpperLimitPrice;
        marketData.LowerLimitPrice = pBaseInfo->LowerLimitPrice;
    }

    if (pStaticInfo)
    {
        marketData.OpenPrice = pStaticInfo->OpenPrice;
        marketData.ClosePrice = pStaticInfo->ClosePrice;
        marketData.SettlementPrice = pStaticInfo->SettlementPrice;
        marketData.HighestPrice = pStaticInfo->HighestPrice;
        marketData.LowestPrice = pStaticInfo->LowestPrice;
    }


    ///TODO  lv2行情转换

    PUB_BIZ_MSG_TO_PLUGIN(m_FlowManager, TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_SNAPSHOT_RTN, 0, &marketData, sizeof(CMarketDataExtField), 1);

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

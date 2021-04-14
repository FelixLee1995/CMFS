#include "apiWrapper/ctp/MyMarketSpi.h"
#include <chrono>
#include <fstream>
#include <vector>
#include "adapter/IAdapter.h"
#include "api/ctp_ext/ctp_ftdc_proto.h"
#include "core/singleton.h"
#include "utils/Utils.h"

using namespace ctp_ftd;

MyMarketSpi::MyMarketSpi(MyMarketApi *api) : m_api_(api)
{
    m_FlowManager.reset(Singleton<CFlowManager>::GetInstance());
    m_MarketDataManager.reset(Singleton<CMarketDataManager>::GetInstance());
}

MyMarketSpi::~MyMarketSpi() {}

void MyMarketSpi::OnFrontConnected()
{
    std::cout << "TradeFront connected!" << std::endl;
    m_api_->m_front_connect_stautus.store(true);
}

void MyMarketSpi::OnFrontDisconnected(int nReason)
{
    std::cout << "TradeFront disconnected!" << std::endl;
    m_api_->m_front_connect_stautus.store(false);
}

void MyMarketSpi::OnRspUserLogin(
    CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID == 0)
    {
        std::cout << "Login Success!" << std::endl;
        m_api_->m_login_stautus.store(true);
        return;
    }

    m_api_->m_login_stautus.store(true);
    std::cout << "Login Failed" << std::endl;
}

void MyMarketSpi::OnRspUserLogout(
    CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID == 0)
    {
        std::cout << "Logout Success!" << std::endl;
        return;
    }

    std::cout << "Logout Failed" << std::endl;
}

void MyMarketSpi::OnRspSubMarketData(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    //std::cout << "OnRspSubMarketData!" << std::endl;

    if (pRspInfo && pRspInfo->ErrorID == 0)
    {
        if (pSpecificInstrument)
        {
            //std::cout << "subscribe success: " << pSpecificInstrument->InstrumentID << std::endl;
        }
    }

    if (!pSpecificInstrument)
    {
        std::cout << "OnRspSubMarketData  Failed,  no pSpecificInstrument" << std::endl;
    }
}

void MyMarketSpi::OnRspUnSubMarketData(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    //std::cout << "OnRspUnSubMarketData!" << std::endl;

    if (pRspInfo && pRspInfo->ErrorID == 0)
    {
        if (pSpecificInstrument)
        {
            //std::cout << "unsubscribe success: " << pSpecificInstrument->InstrumentID << std::endl;
        }
    }
}

void MyMarketSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
    // SPDLOG_INFO("market record, instrid: {}, price: {}, vol: {}, updateTime: {}", pDepthMarketData->InstrumentID,
    //     pDepthMarketData->LastPrice, pDepthMarketData->Volume, pDepthMarketData->UpdateTime);

    CMarketDataExtField marketDataOut;

    m_MarketDataManager->UpdateMarketData(pDepthMarketData->InstrumentID, [&](CMarketDataExtField &marketData) {
        strcpy(marketData.ExchangeID, pDepthMarketData->ExchangeID);
        strcpy(marketData.InstrumentID, pDepthMarketData->InstrumentID);

        marketData.LastPrice = MY_HTONF(pDepthMarketData->LastPrice);
        marketData.Turnover = MY_HTONF(pDepthMarketData->Turnover);
        marketData.OpenInterest = MY_HTONF(pDepthMarketData->OpenInterest);
        marketData.Volume = htonl(pDepthMarketData->Volume);

        strcpy(marketData.TradingDay, pDepthMarketData->TradingDay);
        marketData.PreSettlementPrice = MY_HTONF(pDepthMarketData->PreSettlementPrice);
        marketData.PreClosePrice = MY_HTONF(pDepthMarketData->PreClosePrice);
        marketData.PreOpenInterest = MY_HTONF(pDepthMarketData->PreOpenInterest);
        marketData.UpperLimitPrice = MY_HTONF(pDepthMarketData->UpperLimitPrice);
        marketData.LowerLimitPrice = MY_HTONF(pDepthMarketData->LowerLimitPrice);

        marketData.OpenPrice = MY_HTONF(pDepthMarketData->OpenPrice);
        marketData.ClosePrice = MY_HTONF(pDepthMarketData->ClosePrice);
        marketData.SettlementPrice = MY_HTONF(pDepthMarketData->SettlementPrice);
        marketData.HighestPrice = MY_HTONF(pDepthMarketData->HighestPrice);
        marketData.LowestPrice = MY_HTONF(pDepthMarketData->LowestPrice);

        marketData.BidPrice1 = MY_HTONF(pDepthMarketData->BidPrice1);
        marketData.BidVolume1 = MY_HTONF(pDepthMarketData->BidVolume1);
        marketData.AskPrice1 = MY_HTONF(pDepthMarketData->AskPrice1);
        marketData.AskVolume1 = MY_HTONF(pDepthMarketData->AskVolume1);

        marketData.BidPrice2 = MY_HTONF(pDepthMarketData->BidPrice2);
        marketData.BidVolume2 = MY_HTONF(pDepthMarketData->BidVolume2);
        marketData.AskPrice2 = MY_HTONF(pDepthMarketData->AskPrice2);
        marketData.AskVolume2 = MY_HTONF(pDepthMarketData->AskVolume2);

        marketData.BidPrice3 = MY_HTONF(pDepthMarketData->BidPrice3);
        marketData.BidVolume3 = MY_HTONF(pDepthMarketData->BidVolume3);
        marketData.AskPrice3 = MY_HTONF(pDepthMarketData->AskPrice3);
        marketData.AskVolume3 = MY_HTONF(pDepthMarketData->AskVolume3);

        marketData.BidPrice4 = MY_HTONF(pDepthMarketData->BidPrice4);
        marketData.BidVolume4 = MY_HTONF(pDepthMarketData->BidVolume4);
        marketData.AskPrice4 = MY_HTONF(pDepthMarketData->AskPrice4);
        marketData.AskVolume4 = MY_HTONF(pDepthMarketData->AskVolume4);

        marketData.BidPrice5 = MY_HTONF(pDepthMarketData->BidPrice5);
        marketData.BidVolume5 = MY_HTONF(pDepthMarketData->BidVolume5);
        marketData.AskPrice5 = MY_HTONF(pDepthMarketData->AskPrice5);
        marketData.AskVolume5 = MY_HTONF(pDepthMarketData->AskVolume5);

        strcpy(marketData.UpdateTime, pDepthMarketData->UpdateTime);
        strcpy(marketData.ActionDay, pDepthMarketData->ActionDay);

        marketDataOut = marketData;
    });

    m_FlowManager->PubBizMsg2Plugin(TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_SNAPSHOT_RTN, 0,
        reinterpret_cast<const char *>(&marketDataOut), sizeof(CMarketDataExtField), 1);
}

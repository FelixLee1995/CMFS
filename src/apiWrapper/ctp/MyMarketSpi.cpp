#include <fstream>
#include <chrono>
#include <vector>
#include "apiWrapper/ctp/MyMarketSpi.h"
#include "api/ctp_ext/ctp_ftdc_proto.h"
#include "adapter/IAdapter.h"
#include "core/singleton.h"
#include "utils/Utils.h"

using namespace ctp_ftd;

MyMarketSpi::MyMarketSpi(MyMarketApi *api) : m_api_(api) 
{

    m_FlowManager.reset(Singleton<CFlowManager>::GetInstance());

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
    std::cout << "OnRspSubMarketData!" << std::endl;

    if (pRspInfo && pRspInfo->ErrorID == 0)
    {
        if (pSpecificInstrument)
        {
            std::cout << "subscribe success: " << pSpecificInstrument->InstrumentID << std::endl;
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
    std::cout << "OnRspUnSubMarketData!" << std::endl;

    if (pRspInfo && pRspInfo->ErrorID == 0)
    {
        if (pSpecificInstrument)
        {
            std::cout << "unsubscribe success: " << pSpecificInstrument->InstrumentID << std::endl;
        }
    }
}

void MyMarketSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
    SPDLOG_INFO("market record, instrid: {}, price: {}, vol: {}, updateTime: {}", pDepthMarketData->InstrumentID,
        pDepthMarketData->LastPrice, pDepthMarketData->Volume, pDepthMarketData->UpdateTime);

    CMarketDataExtField marketData;
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

    strcpy(marketData.UpdateTime, pDepthMarketData->UpdateTime);
    strcpy(marketData.ActionDay, pDepthMarketData->ActionDay);

    PUB_BIZ_MSG_TO_PLUGIN(m_FlowManager, TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_SNAPSHOT_RTN, 0, &marketData, sizeof(CMarketDataExtField), 1);


}

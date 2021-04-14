//
// Created by felix on 2021/3/9.
//

#include "adapter/CMockAdapter.h"
#include "api/ctp/ThostFtdcMdApi.h"
#include "core/plantformtools.h"
#include <random>
#include "utils/Utils.h"
#include "api/ctp_ext/ctp_ext.h"
#include "api/ctp_ext/ctp_ftdc_proto.h"

using namespace ctp_ftd;

CMockAdapter::CMockAdapter():  IMarketRecvAdapter("MockAdapter"), m_IfWorking(false)
{
        SPDLOG_INFO("start MockAdapter");

}

void CMockAdapter::Init()
{
    m_IfWorking.store(true);
    m_SendThread = std::make_shared<std::thread>([this] { SendThread(); });
}


void CMockAdapter::Stop()
{
    m_IfWorking.store(false);
    if (m_SendThread->joinable())
    {
        m_SendThread->join();
    }
    m_SendThread.reset();
}

void CMockAdapter::SendThread()
{
    std::random_device rd;  // 将用于为随机数引擎获得种子
    std::mt19937 gen(rd()); // 以播种标准 mersenne_twister_engine
    std::uniform_int_distribution<> dis(1, 128);

    CMarketDataExtField marketDataField;
    while (m_IfWorking)
    {
        marketDataField.LastPrice = MY_HTONF(dis(gen));
        strcpy(marketDataField.InstrumentID, "a2105");
        strcpy(marketDataField.TradingDay, "20210310");
        strcpy(marketDataField.ExchangeID, "DCE");
        marketDataField.OpenPrice = MY_HTONF(127);
        marketDataField.Volume = htonl(2);
        marketDataField.HighestPrice = MY_HTONF(128);
        marketDataField.AveragePrice = MY_HTONF(50);
        marketDataField.ClosePrice = MY_HTONF(48);
        strcpy(marketDataField.UpdateTime, GetFormatTimeStr9().c_str());
        //strcpy(marketDataField.extField, marketDataField.InstrumentID);

        struct timeval time;
        gettimeofday(&time, NULL);
        marketDataField.UpdateMillisec =  htonl(time.tv_usec/1000);

        m_FlowManagerSptr->PubBizMsg2Plugin(TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_SNAPSHOT_RTN, 0, reinterpret_cast<const char*>(&marketDataField), sizeof(CMarketDataExtField), 1);
        CommonSleep(1000);
    }
}

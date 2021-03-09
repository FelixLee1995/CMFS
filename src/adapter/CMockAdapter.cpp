//
// Created by felix on 2021/3/9.
//

#include "adapter/CMockAdapter.h"
#include "api/ctp/ThostFtdcMdApi.h"
#include "core/plantformtools.h"
#include <random>

CMockAdapter::CMockAdapter():  IMarketRecvAdapter("MockAdapter"), m_IfWorking(false)
{

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

    static CThostFtdcDepthMarketDataField marketDataField;
    while (m_IfWorking)
    {
        SPDLOG_INFO("send thread working");
        marketDataField.LastPrice = dis(gen);

        PUB_BIZ_MSG_TO_PLUGIN(m_FlowManagerSptr, TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_SNAPSHOT_RTN, 0, &marketDataField, sizeof(CThostFtdcDepthMarketDataField), 1);
        CommonSleep(2)
    }
}

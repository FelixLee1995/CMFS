#include "adapter/CCtpMarketAdapter.h"
#include "plugin/CMarketDataManager.h"
#include "core/plantformtools.h"


CCtpMarketAdapter::CCtpMarketAdapter(const nlohmann::json &config)
: IMarketRecvAdapter("CtpMarketAdapter")
{
    m_api = std::make_shared<MyMarketApi>(config);

}

CCtpMarketAdapter::~CCtpMarketAdapter()
{
    m_api->Dispose();
    m_api.reset();
}


void CCtpMarketAdapter::Init()
{
    m_api->Init();

    CommonSleep(1000);

    ///TODO  先使用断言判断，  后续将启动状态纳入监控
    if (m_api->GetConnectStatus() == false)
    {   
        std::cout << "ctpMarketAdapter init failed" << std::endl;
        return;
    }
    assert(m_api->GetConnectStatus() == true);


    m_api->ReqUserLogin();
    CommonSleep(1000);

    // 先使用断言判断，  后续将启动状态纳入监控
    assert(m_api->GetLoginStatus() == true);


    auto marketDataManager = Singleton<CMarketDataManager>::Instance();

    assert(marketDataManager);

    marketDataManager->LockedIterFunc([&](const MarketData& market){
        m_api->ReqSubscribeMarketData(market.Data.InstrumentID);
    });
    

}


void CCtpMarketAdapter::Stop()
{

}
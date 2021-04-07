#include <iostream>
#include "Init.h"
#include "core/baseheader.h"
#include "core/singleton.h"
#include "tcp/CTcpServer.h"
#include "plugin/CUserManagePlugin.h"
#include "plugin/CMarketPlugin.h"
#include "adapter/CAdapterManager.h"
#include "monitor/IMonitorApp.h"

int main()
{
    std::cout << "Hello World!" << std::endl;


    mi::factory_func();

    InitEnv();

    asio::io_context ctx;

    auto configIns = Singleton<CConfig>::GetInstance();

    auto port = configIns->LookupConfigWithFlatName<int>("/front/port", 0);


    auto tcpServerPtr = Singleton<CTcpServer>::Instance(1024, port, ctx);

 


    Singleton<CUserSessionManager>::Instance(MAX_ONLINE_USERS);
    Singleton<CMarketDataManager>::Instance();
    auto adapters = Singleton<CAdapterManager>::Instance();

    auto userManagePluginPtr = Singleton<CUserManagePlugin>::Instance();
    auto marketPluginPtr = Singleton<CMarketPlugin>::Instance();

    userManagePluginPtr->Init();
    marketPluginPtr->Init();
    tcpServerPtr->Start();

    adapters->Init();

    ctx.run();

    SPDLOG_INFO("main thread after start");

    return 0;
}
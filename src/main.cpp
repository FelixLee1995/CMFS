#include <iostream>
#include "core/baseheader.h"
#include "core/singleton.h"
#include "tcp/CTcpServer.h"
#include "plugin/CUserManagePlugin.h"
#include "plugin/CMarketPlugin.h"

int main()
{
    std::cout << "Hello World!" << std::endl;

    InitEnv();

    asio::io_context ctx;

    auto configIns = Singleton<CConfig>::GetInstance();

    auto port = configIns->LookupConfigWithFlatName<int>("/front/port", 0);


    Singleton<CUserManagePlugin>::Instance()->Init();
    Singleton<CMarketPlugin>::Instance()->Init();



    Singleton<CTcpServer>::Instance(1024, port, ctx)->Start();

    ctx.run();

    SPDLOG_INFO("main thread after start");

    return 0;
}
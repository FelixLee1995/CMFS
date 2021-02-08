#include <iostream>
#include "core/baseheader.h"
#include "tcp/CTcpServer.h"

int main()
{

    std::cout << "Hello World!" << std::endl;

    InitEnv();

    int res =  100 / 0 ;

    CTcpServer server(1024, 7001);

    server.Start();

    SPDLOG_INFO("main thread after start"); 
    

    return 0;
}
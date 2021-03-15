#include "adapter/CUDPAdapter.h"




CUdpMarketAdapter::CUdpMarketAdapter()
: IMarketRecvAdapter("UdpMarketAdapter")
{

}


CUdpMarketAdapter::~CUdpMarketAdapter()
{

}




void CUdpMarketAdapter::Init()
{   
    api = new MyUdpApi();
    api->Init();
}


void CUdpMarketAdapter::Stop()
{   

}




void MyUdpApi::Init()
{

    pApi = GtjaMdUserApi::CreateMdUserApi("api.log");
	pApi->RegisterSpi(this);
	pApi->RegisterFront("tcp://10.72.3.105:42117");
    pApi->SetConfig("efvi_nic", "enp23s0f0");
	pApi->Init();
	//pApi->Join();

}

void MyUdpApi::OnFrontConnected()
{
    std::cout << "OnFrontConnected ===========================" << std::endl;
    GtjaMdReqUserLoginField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.UserID, "00000001");
    strcpy(req.Password, "88888888");
    pApi->ReqUserLogin(&req, 0);
}
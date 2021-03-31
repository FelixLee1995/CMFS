/**
*  File: CUDPAdapter.h
*  Desc: 自建UDP行情接收适配器
*  Created by felix on 2021/03/15
*/

#ifndef _UDP_ADAPTER_H_
#define _UDP_ADAPTER_H_
#pragma once
#include "api/udp_market/GtjaMdStructV3.h"
#include "api/udp_market/GtjaMdUserApi.h"
#include "api/ctp_ext/ctp_ftdc_proto.h"
#include "IAdapter.h"
#include <string>
#include <iomanip>
#include "plugin/CMarketDataManager.h"


using namespace GtjaMdV3;
using namespace ctp_ftd;

class MyUdpApi: public GtjaMdUserSpi
{
private:
    GtjaMdUserApi* pApi = NULL;
	CFlowManager::Sptr m_FlowManager;
    CMarketDataManager::Sptr m_MarketDataManager;
    std::string m_Path;
    std::string m_Username;
    std::string m_Pwd;
    // const char* DspExch(uint8_t t) {
    // 	switch (t) {
    // 	case _et_czce:
    // 		return "CZCE.";
    // 		break;
    // 	case _et_dce:
    // 		return "DCE.";
    // 		break;
    // 	case _et_shfe:
    // 		return "SHFE.";
    // 		break;
    // 	case _et_cffex:
    // 		return "CFFEX.";
    // 		break;
    // 	case _et_ine:
    // 		return "INE.";
    // 		break;
    // 	case _et_sse:
    // 		return "SSE.";
    // 		break;
    // 	case _et_szse:
    // 		return "SZSE.";
    // 		break;
    // 	case _et_sge:
    // 		return "SGE.";
    // 		break;
    // 	default:
    // 		return "";
    // 	}
    // }
public:

	explicit MyUdpApi(const nlohmann::json &config);
	
    void Init();

    ///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
    void GTJAMDAPI OnFrontConnected() override;

    ///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
    ///@param nReason 错误原因
    ///        0x1001 网络读失败
    ///        0x1002 网络写失败
    ///        0x2001 接收心跳超时
    ///        0x2002 发送心跳失败
    ///        0x2003 收到错误报文
    virtual void GTJAMDAPI OnFrontDisconnected(int nReason){}
    ///错误应答
    virtual void GTJAMDAPI OnRspError(const GtjaMdRspInfoField* pRspInfo, int nRequestID, bool bIsLast){

    }
    ///登录请求响应
    void GTJAMDAPI OnRspUserLogin(const GtjaMdRspUserLoginField* pRspUserLogin, const GtjaMdRspInfoField* pRspInfo, int nRequestID, bool bIsLast) override;
    ///深度行情通知
    void GTJAMDAPI OnRtnDepthSnapshot(
        const GtjaMdV3::GtjaMdInstrumentFieldV3* pInstrument,
        const GtjaMdV3::TGtjaDateTime* pStamp,
        const GtjaMdV3::GtjaMdTradeInfoFieldV3* pTradeInfo,
        const GtjaMdV3::GtjaMdBaseInfoFieldV3* pBaseInfo,
        const GtjaMdV3::GtjaMdStaticInfoFieldV3* pStaticInfo,
        int MBLLength,
        const  GtjaMdV3::GtjaMdMBLFieldV3* pMBL) override;

    void GTJAMDAPI OnRspLastSnapshot(const GtjaDepthMarketDataField* pDepthMarketData) override;


    std::string ConvertExchange(uint8_t exchange);
};



class CUdpMarketAdapter: public IMarketRecvAdapter 
{
    private:
    MyUdpApi * api;

    public:
    explicit CUdpMarketAdapter(const nlohmann::json &config);
    void Init() override;
    void Stop() override;
    ~CUdpMarketAdapter();
};






#endif
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
#include "IAdapter.h"
#include <string>
#include <math.h>
#include <iomanip>


using namespace GtjaMdV3;

class MyUdpApi;

class CUdpMarketAdapter: public IMarketRecvAdapter 
{
    private:
    MyUdpApi * api;
    public:
    CUdpMarketAdapter();
    void Init() override;
    void Stop() override;
    ~CUdpMarketAdapter();
};




class MyUdpApi: public GtjaMdUserSpi
{
private:
    GtjaMdUserApi* pApi = NULL; 
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
	virtual void GTJAMDAPI OnRspUserLogin(const GtjaMdRspUserLoginField* pRspUserLogin, const GtjaMdRspInfoField* pRspInfo, int nRequestID, bool bIsLast) {
		if (pRspInfo && pRspInfo->ErrorID){
			printf("RspUserLogin,Error:%d,%s\n",pRspInfo->ErrorID,pRspInfo->ErrorMsg);
		}
		else if (pRspUserLogin){
			printf("RspUserLogin Succ:LoginTime:%s\n",pRspUserLogin->LoginTime);
		}
	}
	///深度行情通知
	virtual void GTJAMDAPI OnRtnDepthSnapshot(
		const GtjaMdV3::GtjaMdInstrumentFieldV3* pInstrument,
		const GtjaMdV3::TGtjaDateTime* pStamp,
		const GtjaMdV3::GtjaMdTradeInfoFieldV3* pTradeInfo,
		const GtjaMdV3::GtjaMdBaseInfoFieldV3* pBaseInfo,
		const GtjaMdV3::GtjaMdStaticInfoFieldV3* pStaticInfo,
		int MBLLength,
		const  GtjaMdV3::GtjaMdMBLFieldV3* pMBL)
	{

		std::cout <<"OnRtnDepthMarketData: "<< pInstrument->ExchangeType <<pInstrument->InstrumentID<<"\t" << std::setfill('0') << std::setw(2) <<(int)pStamp->Time.Hour<<":"<<(int)pStamp->Time.Minite<<":"<<pStamp->Time.Seccond<<"." << std::setw(3) <<pStamp->Time.MilliSec;
		if (pTradeInfo){
			std::cout <<"\t"<<pTradeInfo->LastPrice \
			<<"\t"<<pTradeInfo->Volume;
		}
		if (pMBL && MBLLength>0){
			std::cout <<"\t"<<pMBL[0].BidPrice<<"\t"<<pMBL[0].BidVolume<<"\t"<<pMBL[0].AskPrice \
			<<"\t"<<pMBL[0].AskVolume;
		}

	}
	virtual void GTJAMDAPI OnRspLastSnapshot(const GtjaDepthMarketDataField* pDepthMarketData){
	
		std::cout <<"OnRspLastSnapshot:"<<pDepthMarketData->InstrumentID<<"\t"<<pDepthMarketData->UpdateTime<<"."<<pDepthMarketData->UpdateMillisec;
		std::cout <<"\t"<<pDepthMarketData->LastPrice \
			<<"\t"<<pDepthMarketData->Volume;
		std::cout <<"\t"<<pDepthMarketData->BidPrice1<<"\t"<<pDepthMarketData->BidVolume1<<"\t"<<pDepthMarketData->AskPrice1 \
			<<"\t"<<pDepthMarketData->AskVolume1;

	}
};



#endif
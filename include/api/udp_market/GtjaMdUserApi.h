/////////////////////////////////////////////////////////////////////////
///@system UDP�����������
///@company ��̩�����ڻ����޹�˾
///@file GtjaMdStruct.h
///@history 
///20060621	��ΰ��		�������ļ�
/////////////////////////////////////////////////////////////////////////

#if !defined(__GTJAMDUSERAPI_H__)
#define __GTJAMDUSERAPI_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GtjaMdStructV3.h"

#if defined(_WINDOWS_) || defined(WIN32)
#ifdef GTJAMDUSERAPI_EXPORTS
#define GTJAMD_API_EXPORT __declspec(dllexport)
#else
#define GTJAMD_API_EXPORT __declspec(dllimport)
#endif
#define GTJAMDAPI __stdcall
#else
#define GTJAMD_API_EXPORT
#define GTJAMDAPI
#endif

class GtjaMdUserSpi
{
public:
	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void GTJAMDAPI OnFrontConnected(){};

	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	///@param nReason ����ԭ��
	///        0x1001 �����ʧ��
	///        0x1002 ����дʧ��
	///        0x2001 ����������ʱ
	///        0x2002 ��������ʧ��
	///        0x2003 �յ�������
	virtual void GTJAMDAPI OnFrontDisconnected(int /*nReason*/){};
	///����Ӧ��
	virtual void GTJAMDAPI OnRspError(const GtjaMdV3::GtjaMdRspInfoField* /*pRspInfo*/, int /*nRequestID*/, bool /*bIsLast*/){};
	///��¼������Ӧ
	virtual void GTJAMDAPI OnRspUserLogin(const GtjaMdV3::GtjaMdRspUserLoginField* /*pRspUserLogin*/, const GtjaMdV3::GtjaMdRspInfoField* /*pRspInfo*/, int /*nRequestID*/, bool /*bIsLast*/) = 0;
	///�������֪ͨ
	virtual void GTJAMDAPI OnRtnDepthSnapshot(
		const GtjaMdV3::GtjaMdInstrumentFieldV3* /*pInstrument*/,
		const GtjaMdV3::TGtjaDateTime* /*pStamp*/,
		const GtjaMdV3::GtjaMdTradeInfoFieldV3* /*pTradeInfo*/,
		const GtjaMdV3::GtjaMdBaseInfoFieldV3* /*pBaseInfo*/,
		const GtjaMdV3::GtjaMdStaticInfoFieldV3* /*pStaticInfo*/,
		int /*MBLLength*/,
		const  GtjaMdV3::GtjaMdMBLFieldV3* /*pMBL*/) = 0;
	//��¼�󷵻����һ�εĿ�����Ϣ����ѡ���ã���һ���л��з��أ�
	virtual void GTJAMDAPI OnRspLastSnapshot(const GtjaMdV3::GtjaDepthMarketDataField* /*pDepthMarketData*/){};
};

class GTJAMD_API_EXPORT GtjaMdUserApi{
protected:
	virtual GTJAMDAPI ~GtjaMdUserApi(){}
public:
	//����ʵ��
	static GtjaMdUserApi* GTJAMDAPI CreateMdUserApi(const char* logPath);

	//ɾ���ӿڶ�����
	///@remark ����ʹ�ñ��ӿڶ���ʱ,���øú���ɾ���ӿڶ���
	virtual void GTJAMDAPI Release() = 0;

	///��ʼ��
	///@remark ��ʼ�����л���,ֻ�е��ú�,�ӿڲſ�ʼ����
	virtual int GTJAMDAPI Init() = 0;

	///�ȴ��ӿ��߳̽�������
	///@return �߳��˳�����
	virtual int GTJAMDAPI Join() = 0;

	///ע��ǰ�û������ַ
	///@param pszFrontAddress��ǰ�û������ַ��
	///@remark �����ַ�ĸ�ʽΪ����protocol://ipaddress:port�����磺��tcp://127.0.0.1:17001���� 
	///@remark ��tcp��������Э�飬��127.0.0.1�������������ַ����17001������������˿ںš�
	virtual void GTJAMDAPI RegisterFront(const char *pszFrontAddress) = 0;

	///ע��ص��ӿ�
	///@param pSpi �����Իص��ӿ����ʵ��
	virtual void GTJAMDAPI RegisterSpi(GtjaMdUserSpi *pSpi) = 0;

	///������������(Ԥ���ӿ�)
	///@param key:������key��
	///@param val:������value��
	///@remark �ú�������Init����֮ǰ���ã�Init֮�������Ч
	virtual void GTJAMDAPI SetConfig(const char* key, const char* val) = 0;

	///�û���¼����
	virtual int GTJAMDAPI ReqUserLogin(const GtjaMdV3::GtjaMdReqUserLoginField *pReqUserLoginField, int nRequestID) = 0;

};

#endif //__GTJAMDUSERAPI_H__

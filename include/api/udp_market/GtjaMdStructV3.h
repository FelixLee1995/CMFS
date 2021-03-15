/////////////////////////////////////////////////////////////////////////////////////////////////////////
///@system UDP�����������
///@company ��̩�����ڻ����޹�˾
///@file GtjaMdStructV3.h
///@version v3.0
///@history 
///20190304	��ΰ��		�������ļ�
///@��ʽ˵��
///�������ʽΪ|GtjaMdHeaderField|...�����ṹ��...|
///�����ṹ��ΪGtjaMdBaseInfoField��GtjaMdStaticInfoField��GtjaMdTradeInfoField��GtjaMdMBLField�е��������
///���ṹ�����ʼ��ַȡ����GtjaMdHeaderField�е�TradeInfoIdx,MBLInfoIdx,MBLCount,StaticInfoIdx,BaseInfoIndx
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(__GTJAMDSTRUCT_V3_H__)
#define __GTJAMDSTRUCT_V3_H__
#include <stdint.h>

#ifdef GTJAMD_PROTOCOL_VERSION
#undef GTJAMD_PROTOCOL_VERSION
#endif
#define GTJAMD_PROTOCOL_VERSION 6

#pragma pack(push,1)

#ifdef __cplusplus
namespace GtjaMdV3{
#endif

//////////////////////////////////////////////////////////////////////////
//���½ṹ�����ڵ�¼���񼰿�������
//////////////////////////////////////////////////////////////////////////
struct GtjaMdRspInfoField{
	int ErrorID;
	char ErrorMsg[44];
};
struct GtjaMdReqUserLoginField{
	char UserID[16];
	char Password[24];
	int32_t ProtocolVersion;
};
struct GtjaMdRspUserLoginField{
	char UserID[16];
	char LoginTime[8];//��¼ʱ�䣺�����ʱ�䣬�����ο�����һ��׼
};
struct GtjaMdReqUserPasswordUpdateField{
	char UserID[16];
	char OldPassword[24];
	char NewPassword[24];
};
struct GtjaMdReqQryDepthMarketDataField{
	char UserID[16];
};
struct GtjaDepthMarketDataField
{
	///������
	char	TradingDay[9];
	///��Լ����
	char	InstrumentID[31];
	///���¼�
	double	LastPrice;
	///�ϴν����
	double	PreSettlementPrice;
	///������
	double	PreClosePrice;
	///��ֲ���
	double	PreOpenInterest;
	///����
	double	OpenPrice;
	///����
	int64_t Volume;
	///�ɽ����
	double	Turnover;
	///�ֲ���
	double	OpenInterest;
	///������
	double	ClosePrice;
	///���ν����
	double	SettlementPrice;
	///��ͣ���
	double	UpperLimitPrice;
	///��ͣ���
	double	LowerLimitPrice;
	///����޸�ʱ��
	char	UpdateTime[9];
	///����޸ĺ���
	int32_t	UpdateMillisec;
	///�����һ
	double	BidPrice1;
	///������һ
	int32_t	BidVolume1;
	///������һ
	double	AskPrice1;
	///������һ
	int32_t	AskVolume1;
	///����۶�
	double	BidPrice2;
	///��������
	int32_t	BidVolume2;
	///�����۶�
	double	AskPrice2;
	///��������
	int32_t	AskVolume2;
	///�������
	double	BidPrice3;
	///��������
	int32_t	BidVolume3;
	///��������
	double	AskPrice3;
	///��������
	int32_t	AskVolume3;
	///�������
	double	BidPrice4;
	///��������
	int32_t	BidVolume4;
	///��������
	double	AskPrice4;
	///��������
	int32_t	AskVolume4;
	///�������
	double	BidPrice5;
	///��������
	int32_t	BidVolume5;
	///��������
	double	AskPrice5;
	///��������
	int32_t	AskVolume5;
	///ҵ������
	char	ActionDay[9];
};

//////////////////////////////////////////////////////////////////////////
//��������ʵʱ������������
//////////////////////////////////////////////////////////////////////////
#ifndef _DEF_GTJA_DATETIME_
#define _DEF_GTJA_DATETIME_
typedef union TGtjaDate{
	struct{
		uint8_t		Day;	//����ʱ�������
		uint8_t		Month;//����ʱ�������
		uint16_t	Year;//����ʱ�������
	};
	uint32_t		Date;	//�������ڣ������տ��Ժϲ���uint32��������С�˸�ʽֵΪ����
} GtjaDate_t;

typedef union TGtjaTime{
	struct{
		uint16_t	Seccond:6;	//����ʱ������뼰����������6bits��ʾ��������10bits��ʾ������
		uint16_t	MilliSec:10; //
		uint8_t		Minite;		//����ʱ�����������
		uint8_t		Hour;		//����ʱ�����Сʱ��
	};
	uint32_t		Time;
} GtjaTime_t;

typedef union TGtjaDateTime{
	struct{
		GtjaTime_t	Time;
		GtjaDate_t	Date;
	};
	uint64_t		DateTime;
} GtjaDateTime_t;
#endif //_DEF_GTJA_DATETIME_

struct GtjaMdHeaderFieldV3
{
	uint8_t FieldLen; //�ýṹ�峤��
	//�������ͣ�0 ����(�ڻ�,�ڻ���Ȩ)���飻1 ������ί�У�2 �����ʳɽ�����
	uint8_t	MdType;
	//Э��汾(�����ڻ����鵱ǰΪ6)
	uint8_t	Version;
	uint8_t unused[1];
	uint32_t SeqNum;//���кţ���ʱ�����壩
	//�������ڼ�ʱ��
	GtjaDateTime_t UpdateDateTime;
	//���ֶ���ʼ��ַ���������ݰ���ͷƫ������Ϊʵ��ƫ������8���������ֵΪ5����ʾ���ֶ�λ�����ݰ���5*8=40�ֽڴ�),ƫ����Ϊ0��ʾ�޴��ֶ�
	uint8_t	InstrumentIdx,TradeInfoIdx,MBLInfoIdx,MBLCount,StaticInfoIdx,BaseInfoIndx;
	uint8_t Reseved[2];
};

struct GtjaMdInstrumentFieldV3{
	uint8_t	ExchangeType;//ref TExchangeType
	char	InstrumentID[1];
};

#ifndef GTJAMD_MILLI_SHIFT
#define GTJAMD_MILLI_SHIFT 10
#endif
#ifndef GTJAMD_MILLI_MASK
#define GTJAMD_MILLI_MASK 0x03ff
#endif

//��������ṹ��
struct GtjaMdBaseInfoFieldV3
{
	///������
	char	TradingDay[8];
	///�ϴν����
	double	PreSettlementPrice;
	///������
	double	PreClosePrice;
	///��ֲ���
	double	PreOpenInterest;
	///��ͣ���
	double	UpperLimitPrice;
	///��ͣ���
	double	LowerLimitPrice;
};

//��̬����ṹ��
struct GtjaMdStaticInfoFieldV3
{
	///����
	double	OpenPrice;
	///������
	double	ClosePrice;
	///���ν����
	double	SettlementPrice;
	///��߼�
	double	HighestPrice;
	///��ͼ�
	double	LowestPrice;
};

//�ɽ���Ϣ�ṹ��
struct GtjaMdTradeInfoFieldV3
{
	///���¼�
	double	LastPrice;
	///�ɽ����
	double	Turnover;
	///�ֲ���
	double	OpenInterest;
	///�ɽ�����
	int64_t Volume;
};

//�̿ڼ�λ�ṹ��
struct GtjaMdMBLFieldV3
{
	///�����
	double	BidPrice;
	///������
	double	AskPrice;
	///������
	int32_t	BidVolume;
	///������
	int32_t	AskVolume;
};

enum TExchangeType
#if (__cplusplus>=201103L)||(_MSC_VER >= 1800)
	:uint8_t
#endif
{
	_et_unset=0,
	_et_czce=1,//֣����
	_et_dce=2,//������
	_et_shfe=3,//������
	_et_cffex=4,//�н���
	_et_ine=5,//��Դ��������
	_et_sse=6,//�Ͻ���
	_et_szse=7,//���
	_et_sge=8,//�Ϻ��ƽ�����
};

#ifdef __cplusplus
}//GtjaMdV3
#endif

#pragma pack(pop)

#endif //__GTJAMDSTRUCT_V3_H__

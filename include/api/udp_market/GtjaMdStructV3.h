/////////////////////////////////////////////////////////////////////////////////////////////////////////
///@system UDP快速行情二期
///@company 国泰君安期货有限公司
///@file GtjaMdStructV3.h
///@version v3.0
///@history 
///20190304	高伟男		创建该文件
///@格式说明
///行情包格式为|GtjaMdHeaderField|...其它结构体...|
///其它结构体为GtjaMdBaseInfoField、GtjaMdStaticInfoField、GtjaMdTradeInfoField、GtjaMdMBLField中的任意组合
///各结构体的起始地址取决于GtjaMdHeaderField中的TradeInfoIdx,MBLInfoIdx,MBLCount,StaticInfoIdx,BaseInfoIndx
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
//以下结构体用于登录服务及快照数据
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
	char LoginTime[8];//登录时间：服务端时间，仅供参考，不一定准
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
	///交易日
	char	TradingDay[9];
	///合约代码
	char	InstrumentID[31];
	///最新价
	double	LastPrice;
	///上次结算价
	double	PreSettlementPrice;
	///昨收盘
	double	PreClosePrice;
	///昨持仓量
	double	PreOpenInterest;
	///今开盘
	double	OpenPrice;
	///数量
	int64_t Volume;
	///成交金额
	double	Turnover;
	///持仓量
	double	OpenInterest;
	///今收盘
	double	ClosePrice;
	///本次结算价
	double	SettlementPrice;
	///涨停板价
	double	UpperLimitPrice;
	///跌停板价
	double	LowerLimitPrice;
	///最后修改时间
	char	UpdateTime[9];
	///最后修改毫秒
	int32_t	UpdateMillisec;
	///申买价一
	double	BidPrice1;
	///申买量一
	int32_t	BidVolume1;
	///申卖价一
	double	AskPrice1;
	///申卖量一
	int32_t	AskVolume1;
	///申买价二
	double	BidPrice2;
	///申买量二
	int32_t	BidVolume2;
	///申卖价二
	double	AskPrice2;
	///申卖量二
	int32_t	AskVolume2;
	///申买价三
	double	BidPrice3;
	///申买量三
	int32_t	BidVolume3;
	///申卖价三
	double	AskPrice3;
	///申卖量三
	int32_t	AskVolume3;
	///申买价四
	double	BidPrice4;
	///申买量四
	int32_t	BidVolume4;
	///申卖价四
	double	AskPrice4;
	///申卖量四
	int32_t	AskVolume4;
	///申买价五
	double	BidPrice5;
	///申买量五
	int32_t	BidVolume5;
	///申卖价五
	double	AskPrice5;
	///申卖量五
	int32_t	AskVolume5;
	///业务日期
	char	ActionDay[9];
};

//////////////////////////////////////////////////////////////////////////
//以下用于实时快照行情数据
//////////////////////////////////////////////////////////////////////////
#ifndef _DEF_GTJA_DATETIME_
#define _DEF_GTJA_DATETIME_
typedef union TGtjaDate{
	struct{
		uint8_t		Day;	//更新时间戳：日
		uint8_t		Month;//更新时间戳：月
		uint16_t	Year;//更新时间戳：年
	};
	uint32_t		Date;	//更新日期：年月日可以合并成uint32整数，按小端格式值为递增
} GtjaDate_t;

typedef union TGtjaTime{
	struct{
		uint16_t	Seccond:6;	//更新时间戳：秒及毫秒数：高6bits表示秒数，低10bits表示毫秒数
		uint16_t	MilliSec:10; //
		uint8_t		Minite;		//更新时间戳：分钟数
		uint8_t		Hour;		//更新时间戳：小时数
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
	uint8_t FieldLen; //该结构体长度
	//行情类型：0 快照(期货,期货期权)行情；1 深交所逐笔委托；2 深交所逐笔成交行情
	uint8_t	MdType;
	//协议版本(对于期货行情当前为6)
	uint8_t	Version;
	uint8_t unused[1];
	uint32_t SeqNum;//序列号（暂时无意义）
	//更新日期及时间
	GtjaDateTime_t UpdateDateTime;
	//各字段起始地址与行情数据包开头偏移量（为实际偏移量除8，即：如果值为5，表示该字段位于数据包的5*8=40字节处),偏移量为0表示无此字段
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

//基础行情结构体
struct GtjaMdBaseInfoFieldV3
{
	///交易日
	char	TradingDay[8];
	///上次结算价
	double	PreSettlementPrice;
	///昨收盘
	double	PreClosePrice;
	///昨持仓量
	double	PreOpenInterest;
	///涨停板价
	double	UpperLimitPrice;
	///跌停板价
	double	LowerLimitPrice;
};

//静态行情结构体
struct GtjaMdStaticInfoFieldV3
{
	///今开盘
	double	OpenPrice;
	///今收盘
	double	ClosePrice;
	///本次结算价
	double	SettlementPrice;
	///最高价
	double	HighestPrice;
	///最低价
	double	LowestPrice;
};

//成交信息结构体
struct GtjaMdTradeInfoFieldV3
{
	///最新价
	double	LastPrice;
	///成交金额
	double	Turnover;
	///持仓量
	double	OpenInterest;
	///成交数量
	int64_t Volume;
};

//盘口价位结构体
struct GtjaMdMBLFieldV3
{
	///申买价
	double	BidPrice;
	///申卖价
	double	AskPrice;
	///申买量
	int32_t	BidVolume;
	///申卖量
	int32_t	AskVolume;
};

enum TExchangeType
#if (__cplusplus>=201103L)||(_MSC_VER >= 1800)
	:uint8_t
#endif
{
	_et_unset=0,
	_et_czce=1,//郑商所
	_et_dce=2,//大商所
	_et_shfe=3,//上期所
	_et_cffex=4,//中金所
	_et_ine=5,//能源交易中心
	_et_sse=6,//上交所
	_et_szse=7,//深交所
	_et_sge=8,//上海黄金交易所
};

#ifdef __cplusplus
}//GtjaMdV3
#endif

#pragma pack(pop)

#endif //__GTJAMDSTRUCT_V3_H__

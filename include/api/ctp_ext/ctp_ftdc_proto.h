#ifndef __CTP_FTDC_PROTO_H__
#define __CTP_FTDC_PROTO_H__
#include <stdint.h>
#include "api/ctp/ThostFtdcUserApiDataType.h"
#include "api/ctp/ThostFtdcUserApiStruct.h"
#if (defined WIN32)||(defined _WINDOWS_)
#pragma pack(push,1)
#elif defined __GNUC__
#pragma pack(push,1)
#endif
#ifdef __cplusplus
namespace ctp_ftd{
	extern "C"{
#endif
	union inner_double{
		uint64_t	u64;
		double		value;
		struct __towpart{
			uint32_t lower;
			uint32_t higher;
		};
	};   
	struct ftd_header
	{
		uint8_t		ftd_type;
		uint8_t		ftd_ext_len;
		uint16_t	ftd_content_len;
	};

	struct ftd_ext_header
	{
		uint8_t	ftd_ext_type;
		uint8_t	ftd_ext_tag_len;
	};

	struct ftdc_crpheader{
		uint8_t protocol; //0x01 for now
		uint8_t method;  //00 无压缩; 03 压缩0
	};

	struct ftdc_header
	{
		uint8_t		ftdc_version;
		char		ftdc_chain;
		uint16_t	ftdc_seq_series;
		uint32_t	ftdc_topic_id;
		uint32_t	ftdc_seq_no;
		uint16_t	ftdc_field_count;   
		uint16_t	ftdc_content_len;
		uint32_t	ftdc_req_id;
	};

	struct ftdc_field_header
	{
		uint16_t	field_id;
		uint16_t	field_len;
	};

#if(__cplusplus>=201103L)
	enum ftdc_version_vals:uint8_t{
#else
	enum ftdc_version_vals{
#endif
		ftdc_version_cur=0x0f,    // 原代码中为0x0c
	};

#if(__cplusplus>=201103L)
	enum ftdc_crp_vals:uint8_t{
#else
	enum ftdc_crp_vals{
#endif
		ftdc_crp_protocol=0x01,
	};

#if(__cplusplus>=201103L)
	enum ftd_chain_type:char{
#else
	enum ftd_chain_type{
#endif
		ftd_chain_single	= 'S',
		ftd_chain_continue	= 'C',
		ftd_chain_last		= 'L',
	};

#if(__cplusplus>=201103L)
	enum ftd_type:uint8_t{
#else
	enum ftd_type{
#endif
		ftd_type_none		= 0x00,
		ftd_type_normal		= 0x01,
		ftd_type_compressed	= 0x02,
	};
#if(__cplusplus>=201103L)
	enum ftd_ext_type:uint8_t{
#else
	enum ftd_ext_type{
#endif
		ftd_ext_type_none			= 0x00,
		ftd_ext_type_datetime		= 0x01,
		ftd_ext_type_compressmethod	= 0x02,
		ftd_ext_type_transactionid	= 0x03,
		ftd_ext_type_sessionstate	= 0x04,
		ftd_ext_type_keepalive		= 0x05,
		ftd_ext_type_tradeday		= 0x06,
		ftd_ext_type_timeout		= 0x07,
	};

#if(__cplusplus>=201103L)
	enum ftdc_sid_type:uint16_t{
#else
	enum ftdc_sid_type{
#endif
		ftdc_sid_none		= 0x00,
		ftdc_sid_dialog		= 0x01,
		ftdc_sid_private	= 0x02,
		ftdc_sid_public		= 0x03,
		ftdc_sid_query		= 0x04,
		ftdc_sid_user		= 0x05,
		ftdc_sid_forquote	= 0x06,
		ftdc_sid_market		= 0x64,//100
	};

#if(__cplusplus>=201103L)
	enum ftdc_compress_type:uint8_t{
#else
	enum ftdc_compress_type{
#endif
		ftdc_cmp_none			= 0x00, //不压缩
		ftdc_cmp_compresszero	= 0x03, //行程压缩0
	};

#if(__cplusplus>=201103L)
	enum ftdc_tid_type:uint32_t{
#else
	enum ftdc_tid_type{
#endif
		ftdc_tid_RspError							= 0x0001,//1
		ftdc_tid_ReqUserLogin						= 0x3000,//12288
		ftdc_tid_RspUserLogin						= 0x3001,//12289
		ftdc_tid_ReqUserLogout						= 0x3002,//12290
		ftdc_tid_RspUserLogout						= 0x3003,//12291
		ftdc_tid_ReqUserPasswordUpdate				= 0x300a,//12298
		ftdc_tid_RspUserPasswordUpdate				= 0x300b,//12299
		ftdc_tid_ReqTradingAccountPasswordUpdate	= 0x300e,//12302
		ftdc_tid_RspTradingAccountPasswordUpdate	= 0x300f,//12303
		ftdc_tid_ReqAuthenticate					= 0x3010,//12304
		ftdc_tid_RspAuthenticate					= 0x3011,//12305
		ftdc_tid_ReqOrderInsert						= 0x4000,//16384
		ftdc_tid_RspOrderInsert						= 0x4001,//16385
		ftdc_tid_ReqParkedOrderInsert				= 0x4002,//16386
		ftdc_tid_RspParkedOrderInsert				= 0x4003,//16387
		ftdc_tid_ReqParkedOrderAction				= 0x4004,//16388
		ftdc_tid_RspParkedOrderAction				= 0x4005,//16389
		ftdc_tid_ReqOrderAction						= 0x4006,//16390
		ftdc_tid_RspOrderAction						= 0x4007,//16391
		ftdc_tid_ReqQueryMaxOrderVolume				= 0x4011,//16401
		ftdc_tid_RspQueryMaxOrderVolume				= 0x4012,//16402
		ftdc_tid_ReqSettlementInfoConfirm			= 0x4013,//16403
		ftdc_tid_RspSettlementInfoConfirm			= 0x4014,//16404
		ftdc_tid_RspFromBankToFutureByFuture_1		= 0x4016,//16406
		ftdc_tid_RspFromFutureToBankByFuture_1		= 0x4018,//16408
		ftdc_tid_RspQueryBankAccountMoneyByFuture_1	= 0x401a,//16410
		ftdc_tid_RspQryTransferSerial_1				= 0x401c,//16412
		ftdc_tid_ReqRemoveParkedOrder				= 0x401d,//16413
		ftdc_tid_RspRemoveParkedOrder				= 0x401f,//16415
		ftdc_tid_ReqRemoveParkedOrderAction			= 0x4020,//16416
		ftdc_tid_RspRemoveParkedOrderAction			= 0x4021,//16417
		ftdc_tid_ReqExecOrderInsert					= 0x4026,//16422
		ftdc_tid_RspExecOrderInsert					= 0x4027,//16423
		ftdc_tid_ReqExecOrderAction					= 0x4028,//16424
		ftdc_tid_RspExecOrderAction					= 0x4029,//16425
		ftdc_tid_ReqForQuoteInsert					= 0x402a,//16426
		ftdc_tid_RspForQuoteInsert					= 0x402b,//16427
		ftdc_tid_ReqQuoteInsert						= 0x402c,//16428
		ftdc_tid_RspQuoteInsert						= 0x402d,//16429
		ftdc_tid_ReqQuoteAction						= 0x402e,//16430
		ftdc_tid_RspQuoteAction						= 0x402f,//16431
		ftdc_tid_ReqCombActionInsert				= 0x4050,//16464
		ftdc_tid_RspCombActionInsert				= 0x4051,//16465
		ftdc_tid_SubscribeMarketData				= 0x4401,//17409
		ftdc_tid_RspSubMarketData					= 0x4402,//17410
		ftdc_tid_UnSubscribeMarketData				= 0x4403,//17411
		ftdc_tid_RspUnSubMarketData					= 0x4404,//17412
		ftdc_tid_SubscribeForQuoteRsp				= 0x4409,//17417
		ftdc_tid_RspSubForQuoteRsp					= 0x440a,//17418
		ftdc_tid_UnSubscribeForQuoteRsp				= 0x440b,//17419
		ftdc_tid_RspUnSubForQuoteRsp				= 0x440c,//17420
		ftdc_tid_ReqQryOrder						= 0x8000,//32768
		ftdc_tid_RspQryOrder						= 0x8001,//32769
		ftdc_tid_ReqQryTrade						= 0x8002,//32770
		ftdc_tid_RspQryTrade						= 0x8003,//32771
		ftdc_tid_ReqQryInvestorPosition				= 0x8004,//32772
		ftdc_tid_RspQryInvestorPosition				= 0x8005,//32773
		ftdc_tid_ReqQryTradingAccount				= 0x8006,//32774
		ftdc_tid_RspQryTradingAccount				= 0x8007,//32775
		ftdc_tid_ReqQryInvestor						= 0x8008,//32776
		ftdc_tid_RspQryInvestor						= 0x8009,//32777
		ftdc_tid_ReqQryTradingCode					= 0x800a,//32778
		ftdc_tid_RspQryTradingCode					= 0x800b,//32779
		ftdc_tid_ReqQryInstrumentMarginRate			= 0x800e,//32782
		ftdc_tid_RspQryInstrumentMarginRate			= 0x800f,//32783
		ftdc_tid_ReqQryInstrumentCommissionRate		= 0x8010,//32784
		ftdc_tid_RspQryInstrumentCommissionRate		= 0x8011,//32785
		ftdc_tid_ReqQryExchange						= 0x802a,//32810
		ftdc_tid_RspQryExchange						= 0x802b,//32811
		ftdc_tid_ReqQryProduct						= 0x802c,//32812
		ftdc_tid_RspQryProduct						= 0x802d,//32813
		ftdc_tid_ReqQryInstrument					= 0x802e,//32814
		ftdc_tid_RspQryInstrument					= 0x802f,//32815
		ftdc_tid_ReqQryDepthMarketData				= 0x8030,//32816
		ftdc_tid_RspQryDepthMarketData				= 0x8031,//32817
		ftdc_tid_ReqQrySettlementInfo				= 0x803c,//32828
		ftdc_tid_RspQrySettlementInfo				= 0x803d,//32829
		ftdc_tid_ReqQryTransferBank					= 0x8048,//32840
		ftdc_tid_RspQryTransferBank					= 0x8049,//32841
		ftdc_tid_ReqQryInvestorPositionDetail		= 0x8050,//32848
		ftdc_tid_RspQryInvestorPositionDetail		= 0x8051,//32849
		ftdc_tid_ReqQryNotice						= 0x8054,//32852
		ftdc_tid_RspQryNotice						= 0x8055,//32853
		ftdc_tid_ReqQrySettlementInfoConfirm		= 0x8056,//32854
		ftdc_tid_RspQrySettlementInfoConfirm		= 0x8057,//32855
		ftdc_tid_ReqQryInvestorPositionCombineDetail= 0x8067,//32871
		ftdc_tid_RspQryInvestorPositionCombineDetail= 0x8068,//32872
		ftdc_tid_ReqQryCFMMCTradingAccountKey		= 0x8077,//32887
		ftdc_tid_RspQryCFMMCTradingAccountKey		= 0x8078,//32888
		ftdc_tid_ReqQryEWarrantOffset				= 0x807b,//32891
		ftdc_tid_RspQryEWarrantOffset				= 0x807c,//32892
		ftdc_tid_ReqQryInvestorProductGroupMargin	= 0x8081,//32897
		ftdc_tid_RspQryInvestorProductGroupMargin	= 0x8082,//32898
		ftdc_tid_ReqQryExchangeMarginRate			= 0x8083,//32899
		ftdc_tid_RspQryExchangeMarginRate			= 0x8084,//32900
		ftdc_tid_ReqQryExchangeMarginRateAdjust		= 0x8085,//32901
		ftdc_tid_RspQryExchangeMarginRateAdjust		= 0x8086,//32902
		ftdc_tid_ReqQryExchangeRate					= 0x8087,//32903
		ftdc_tid_RspQryExchangeRate					= 0x8088,//32904
		ftdc_tid_ReqQrySecAgentACIDMap				= 0x808b,//32907
		ftdc_tid_RspQrySecAgentACIDMap				= 0x808c,//32908
		ftdc_tid_ReqQryProductGroup					= 0x8091,//32913
		ftdc_tid_RspQryProductGroup					= 0x8092,//32914
		ftdc_tid_ReqQryInstrumentOrderCommRate		= 0x8097,//32919
		ftdc_tid_RspQryInstrumentOrderCommRate		= 0x8098,//32920
		ftdc_tid_ReqQryContractBank					= 0x8103,//33027
		ftdc_tid_RspQryContractBank					= 0x8104,//33028
		ftdc_tid_ReqQryParkedOrder					= 0x8105,//33029
		ftdc_tid_RspQryParkedOrder					= 0x8106,//33030
		ftdc_tid_ReqQryParkedOrderAction			= 0x8107,//33031
		ftdc_tid_RspQryParkedOrderAction			= 0x8108,//33032
		ftdc_tid_ReqQryTradingNotice				= 0x8108,//33032, 重复
		ftdc_tid_RspQryTradingNotice				= 0x8109,//33033, 重复
		ftdc_tid_ReqQryBrokerTradingParams			= 0x8109,//33033
		ftdc_tid_RspQryBrokerTradingParams			= 0x810a,//33034
		ftdc_tid_ReqQryBrokerTradingAlgos			= 0x810b,//33035
		ftdc_tid_RspQryBrokerTradingAlgos			= 0x810c,//33036
		ftdc_tid_ReqQueryCFMMCTradingAccountToken	= 0x811b,//33051
		ftdc_tid_RspQueryCFMMCTradingAccountToken	= 0x811c,//33052
		ftdc_tid_ReqQryTransferSerial				= 0x8200,//33280
		ftdc_tid_RspQryTransferSerial				= 0x8201,//33281
		ftdc_tid_ReqQryAccountregister				= 0x8202,//33282
		ftdc_tid_RspQryAccountregister				= 0x8203,//33283
		ftdc_tid_ReqQryOptionInstrTradeCost			= 0x8300,//33536
		ftdc_tid_RspQryOptionInstrTradeCost			= 0x8301,//33537
		ftdc_tid_ReqQryOptionInstrCommRate			= 0x8302,//33538
		ftdc_tid_RspQryOptionInstrCommRate			= 0x8303,//33539
		ftdc_tid_ReqQryExecOrder					= 0x8306,//33542
		ftdc_tid_RspQryExecOrder					= 0x8307,//33543
		ftdc_tid_ReqQryForQuote						= 0x8314,//33556
		ftdc_tid_RspQryForQuote						= 0x8315,//33557
		ftdc_tid_ReqQryQuote						= 0x8318,//33560
		ftdc_tid_RspQryQuote						= 0x8319,//33561
		ftdc_tid_ReqQryCombInstrumentGuard			= 0x8360,//33632
		ftdc_tid_RspQryCombInstrumentGuard			= 0x8361,//33633
		ftdc_tid_ReqQryCombAction					= 0x8362,//33634
		ftdc_tid_RspQryCombAction					= 0x8363,//33635
		
		ftdc_tid_RtnOrder							= 0xf001,//61441
		ftdc_tid_RtnTrade							= 0xf002,//61442
		ftdc_tid_ErrRtnOrderInsert					= 0xf003,//61443
		ftdc_tid_ErrRtnOrderAction					= 0xf004,//61444
		ftdc_tid_ErrRtnParkedOrderInsert			= 0xf005,//61445
		ftdc_tid_ErrRtnParkedOrderAction			= 0xf006,//61446
		ftdc_tid_RtnTradingNotice					= 0xf009,//61449
		ftdc_tid_ErrRtnExecOrderInsert				= 0xf012,//61458
		ftdc_tid_ErrRtnExecOrderAction				= 0xf013,//61459
		ftdc_tid_RtnQuote							= 0xf015,//61461
		ftdc_tid_ErrRtnQuoteInsert					= 0xf016,//61462
		ftdc_tid_ErrRtnQuoteAction					= 0xf017,//61463
		ftdc_tid_ErrRtnCombActionInsert				= 0xf051,//61521
		ftdc_tid_RtnDepthMarketData_snap			= 0xf101,//61697，行情快照
		ftdc_tid_Dissemination						= 0xf102,//61698
		ftdc_tid_RtnDepthMarketData_inc				= 0xf103,//61699, 增量行情
		ftdc_tid_RtnInstrumentStatus				= 0xf104,//61700
		ftdc_tid_ErrRtnQueryBankBalanceByFuture		= 0xf210,//61968

		ftdc_tid_ReqFromBankToFutureByFuture		= 0x1800b,//98315
		ftdc_tid_RspFromBankToFutureByFuture		= 0x1800c,//98316
		ftdc_tid_ReqFromFutureToBankByFuture		= 0x1800d,//98317
		ftdc_tid_RspFromFutureToBankByFuture		= 0x1800e,//98318
		ftdc_tid_ReqQueryBankAccountMoneyByFuture	= 0x18019,//98329
		ftdc_tid_RspQueryBankAccountMoneyByFuture	= 0x1801a,//98330

	};
#if(__cplusplus>=201103L)
	enum ftdc_fid_type:uint16_t{
#else
	enum ftdc_fid_type{
#endif

		ftdc_fid_RspInfoField							= 0x0000,//0
		ftdc_fid_ExchangeField							= 0x0001,//1
		ftdc_fid_ProductField							= 0x0002,//2
		ftdc_fid_InstrumentField						= 0x0003,//3
		ftdc_fid_InvestorField							= 0x0006,//6
		ftdc_fid_TradingCodeField						= 0x0007,//7
		ftdc_fid_TradingAccountField					= 0x000c,//12
		ftdc_fid_InvestorPositionField					= 0x000d,//13
		ftdc_fid_InstrumentMarginRateField				= 0x000e,//14
		ftdc_fid_InstrumentCommissionRateField			= 0x000f,//15
		ftdc_fid_DepthMarketDataField					= 0x0012,//18
		ftdc_fid_SettlementInfoField					= 0x001b,//27
		ftdc_fid_ExchangeMarginRateField				= 0x0021,//33,疑似
		ftdc_fid_ExchangeMarginRateAdjustField			= 0x0022,//34
		ftdc_fid_ExchangeRateField						= 0x0023,//35
		ftdc_fid_UserPasswordUpdateField				= 0x0304,//772
		ftdc_fid_InputOrderField						= 0x0400,//1024
		ftdc_fid_OrderField								= 0x0401,//1025
		ftdc_fid_InputOrderActionField					= 0x0404,//1028
		ftdc_fid_OrderActionField						= 0x0405,//1029
		ftdc_fid_TradeField								= 0x0409,//1033
		ftdc_fid_QueryMaxOrderVolumeField				= 0x040e,//1038
		ftdc_fid_SettlementInfoConfirmField				= 0x040f,//1039
		ftdc_fid_QryOrderField							= 0x0700,//1792
		ftdc_fid_QryTradeField							= 0x0701,//1793
		ftdc_fid_QryInvestorPositionField				= 0x0702,//1794
		ftdc_fid_QryTradingAccountField					= 0x0703,//1795
		ftdc_fid_QryInvestorField						= 0x0704,//1796
		ftdc_fid_QryTradingCodeField					= 0x0705,//1797
		ftdc_fid_QryInstrumentMarginRateField			= 0x0707,//1799
		ftdc_fid_QryInstrumentCommissionRateField		= 0x0708,//1800
		ftdc_fid_QrySettlementInfoField					= 0x070c,//1804
		ftdc_fid_QryExchangeField						= 0x0715,//1813
		ftdc_fid_QryProductField						= 0x0716,//1814
		ftdc_fid_QryInstrumentField						= 0x0717,//1815
		ftdc_fid_QryDepthMarketDataField				= 0x0718,//1816
		ftdc_fid_QryExchangeMarginRateField				= 0x071e,//1822
		ftdc_fid_QryExchangeMarginRateAdjustField		= 0x071f,//1823
		ftdc_fid_QryExchangeRateField					= 0x0720,//1824
		ftdc_fid_OptionInstrCommRateField				= 0x0804,//2052
		ftdc_fid_OptionInstrTradeCostField				= 0x0805,//2053
		ftdc_fid_QryOptionInstrTradeCostField			= 0x0806,//2054
		ftdc_fid_QryOptionInstrCommRateField			= 0x0807,//2055
		ftdc_fid_InputExecOrderField					= 0x080a,//2058
		ftdc_fid_InputExecOrderActionField				= 0x080b,//2059
		ftdc_fid_ExecOrderActionField					= 0x080d,//2061
		ftdc_fid_QryExecOrderField						= 0x080e,//2062
		ftdc_fid_InputForQuoteField						= 0x081a,//2074
		ftdc_fid_ForQuoteField							= 0x081b,//2075
		ftdc_fid_QryForQuoteField						= 0x081c,//2076
		ftdc_fid_InputQuoteField						= 0x081f,//2079
		ftdc_fid_InputQuoteActionField					= 0x0820,//2080
		ftdc_fid_QuoteField								= 0x0821,//2081
		ftdc_fid_QuoteActionField						= 0x0822,//2082
		ftdc_fid_QryQuoteField							= 0x0823,//2083
		ftdc_fid_CombInstrumentGuardField				= 0x0850,//2128
		ftdc_fid_QryCombInstrumentGuardField			= 0x0851,//2129
		ftdc_fid_InputCombActionField					= 0x0852,//2130
		ftdc_fid_QryCombActionField						= 0x0854,//2132
		ftdc_fid_InstrumentOrderCommRateField			= 0x0870,//2160
		ftdc_fid_QryInstrumentOrderCommRateField		= 0x0871,//2161
		ftdc_fid_DisseminationField						= 0x1001,//4097
		ftdc_fid_ReqUserLoginField						= 0x1002,//4098
		ftdc_fid_RspUserLoginField						= 0x1003,//4099
		ftdc_fid_UserLogoutField						= 0x1004,//4100
		ftdc_fid_TransferBankToFutureRspField			= 0x1008,//4104
		ftdc_fid_TransferFutureToBankRspField			= 0x100a,//4106
		ftdc_fid_TransferQryBankRspField				= 0x100c,//4108
		ftdc_fid_AuthenticationInfoField				= 0x1012,//4114

		ftdc_fid_MarketDataBaseField					= 0x2431,//9265
		ftdc_fid_MarketDataStaticField					= 0x2432,//9266
		ftdc_fid_MarketDataLastMatchField				= 0x2433,//9267
		ftdc_fid_MarketDataBestPriceField				= 0x2434,//9268
		ftdc_fid_MarketDataBid23Field					= 0x2435,//9269
		ftdc_fid_MarketDataAsk23Field					= 0x2436,//9270
		ftdc_fid_MarketDataBid45Field					= 0x2437,//9271
		ftdc_fid_MarketDataAsk45Field					= 0x2438,//9272
		ftdc_fid_MarketDataUpdateTimeField				= 0x2439,//9273

		ftdc_fid_SpecificInstrumentField				= 0x2441,//9281
		ftdc_fid_InstrumentStatusField					= 0x2442,//9282
		ftdc_fid_QryTransferBankField					= 0x2447,//9287
		ftdc_fid_TransferBankField						= 0x2448,//9288
		ftdc_fid_QryInvestorPositionDetailField			= 0x2450,//9296
		ftdc_fid_InvestorPositionDetailField			= 0x2451,//9297
		ftdc_fid_QryNoticeField							= 0x2455,//9301
		ftdc_fid_QrySettlementInfoConfirmField			= 0x2458,//9304
		ftdc_fid_TradingAccountPasswordUpdateField		= 0x2460,//9312
		ftdc_fid_QryContractBankField					= 0x2469,//9321
		ftdc_fid_ParkedOrderField						= 0x2473,//9331
		ftdc_fid_ParkedOrderActionField					= 0x2474,//9332
		ftdc_fid_QryParkedOrderField					= 0x2475,//9333
		ftdc_fid_QryParkedOrderActionField				= 0x2476,//9334
		ftdc_fid_RemoveParkedOrderField					= 0x2477,//9335
		ftdc_fid_RemoveParkedOrderActionField			= 0x2478,//9336
		ftdc_fid_QryInvestorPositionCombineDetailField	= 0x2480,//9344
		ftdc_fid_MarketDataAveragePriceField			= 0x2481,//9345, 平均价
		ftdc_fid_TradingNoticeInfoField					= 0x2484,//9348
		ftdc_fid_TradingNoticeField						= 0x2485,//9349
		ftdc_fid_QryTradingNoticeField					= 0x2486,//9350
		ftdc_fid_QryBrokerTradingParamsField			= 0x2494,//9364
		ftdc_fid_BrokerTradingParamsField				= 0x2495,//9365
		ftdc_fid_QryBrokerTradingAlgosField				= 0x2496,//9366
		ftdc_fid_BrokerTradingAlgosField				= 0x2497,//9367
		ftdc_fid_QryCFMMCTradingAccountKeyField			= 0x2503,//9475
		ftdc_fid_QryEWarrantOffsetField					= 0x250a,//9482
		ftdc_fid_QryInvestorProductGroupMarginField		= 0x250d,//9485
		ftdc_fid_QueryCFMMCTradingAccountTokenField		= 0x250f,//9487
		ftdc_fid_QryProductGroupField					= 0x2512,//9490
		ftdc_fid_ProductGroupField						= 0x2513,//9491
		ftdc_fid_ReqTransferField						= 0x2807,//10247
		ftdc_fid_ReqQueryAccountField					= 0x2819,//10265

		ftdc_fid_QryTransferSerialField					= 0x300d,//12301
		ftdc_fid_QryAccountregisterField				= 0x3011,//12305
		ftdc_fid_QrySecAgentACIDMapField				= 0x3017,//12311
	};


	struct CMarketDataExtField
	{
		///交易日
	TThostFtdcDateType	TradingDay;
	///合约代码
	TThostFtdcInstrumentIDType	InstrumentID;
	///交易所代码
	TThostFtdcExchangeIDType	ExchangeID;
	///合约在交易所的代码
	TThostFtdcExchangeInstIDType	ExchangeInstID;
	///最新价
	TThostFtdcPriceType	LastPrice;
	///上次结算价
	TThostFtdcPriceType	PreSettlementPrice;
	///昨收盘
	TThostFtdcPriceType	PreClosePrice;
	///昨持仓量
	TThostFtdcLargeVolumeType	PreOpenInterest;
	///今开盘
	TThostFtdcPriceType	OpenPrice;
	///最高价
	TThostFtdcPriceType	HighestPrice;
	///最低价
	TThostFtdcPriceType	LowestPrice;
	///数量
	TThostFtdcVolumeType	Volume;
	///成交金额
	TThostFtdcMoneyType	Turnover;
	///持仓量
	TThostFtdcLargeVolumeType	OpenInterest;
	///今收盘
	TThostFtdcPriceType	ClosePrice;
	///本次结算价
	TThostFtdcPriceType	SettlementPrice;
	///涨停板价
	TThostFtdcPriceType	UpperLimitPrice;
	///跌停板价
	TThostFtdcPriceType	LowerLimitPrice;
	///昨虚实度
	TThostFtdcRatioType	PreDelta;
	///今虚实度
	TThostFtdcRatioType	CurrDelta;
	///最后修改时间
	TThostFtdcTimeType	UpdateTime;
	///最后修改毫秒
	TThostFtdcMillisecType	UpdateMillisec;
	///申买价一
	TThostFtdcPriceType	BidPrice1;
	///申买量一
	TThostFtdcVolumeType	BidVolume1;
	///申卖价一
	TThostFtdcPriceType	AskPrice1;
	///申卖量一
	TThostFtdcVolumeType	AskVolume1;
	///申买价二
	TThostFtdcPriceType	BidPrice2;
	///申买量二
	TThostFtdcVolumeType	BidVolume2;
	///申卖价二
	TThostFtdcPriceType	AskPrice2;
	///申卖量二
	TThostFtdcVolumeType	AskVolume2;
	///申买价三
	TThostFtdcPriceType	BidPrice3;
	///申买量三
	TThostFtdcVolumeType	BidVolume3;
	///申卖价三
	TThostFtdcPriceType	AskPrice3;
	///申卖量三
	TThostFtdcVolumeType	AskVolume3;
	///申买价四
	TThostFtdcPriceType	BidPrice4;
	///申买量四
	TThostFtdcVolumeType	BidVolume4;
	///申卖价四
	TThostFtdcPriceType	AskPrice4;
	///申卖量四
	TThostFtdcVolumeType	AskVolume4;
	///申买价五
	TThostFtdcPriceType	BidPrice5;
	///申买量五
	TThostFtdcVolumeType	BidVolume5;
	///申卖价五
	TThostFtdcPriceType	AskPrice5;
	///申卖量五
	TThostFtdcVolumeType	AskVolume5;
	///当日均价
	TThostFtdcPriceType	AveragePrice;
	///业务日期
	TThostFtdcDateType	ActionDay;
	char extField[162];
	};





#ifdef __cplusplus
	}
}
#endif

#if (defined WIN32)||(defined _WINDOWS_)
#pragma pack(pop)
#elif defined __GNUC__
#pragma pack(pop)
#endif

#endif //__CTP_FTDC_PROTO_H__

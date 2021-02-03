//
// Created by felix on 2020/9/21.
//  用于定义基础宏
//



#ifndef CDEFINE_H
#define CDEFINE_H

#include <cstdint>
#include "ds/CApiPathDefine.h"


// channel define
const uint32_t CHANNEL_HTTP_CLIENT = 1;
const uint32_t CHANNEL_WS_CLIENT = 2;
const uint32_t CHANNEL_TCP_CLIENT = 3;
const uint32_t PLUGIN_BIZ_AUTH = 4;


// 系统各类环境指标状态
const uint32_t MANAGER_ENV_STATUS = 9;


const uint32_t FUNC_REQ_MARKET_SUB = 2001;
const uint32_t FUNC_RTN_MARKET = 2002;

const uint32_t FUNC_REQ_TRADE_SUB = 2003;
const uint32_t FUNC_RTN_TRADE = 2004;

const uint32_t FUNC_REQ_CONTRACT_SUB = 2005;
const uint32_t FUNC_RTN_SYMBOL = 2006;
const uint32_t FUNC_RTN_CYCLES = 2007;


// func  define
const uint32_t  FUNC_MARKET_AUTH_TOKEN = 1001;

// 标识开始获取auth token，  用于系统初始化或行情断线重连
const uint32_t  FUNC_AUTH_TOKEN_REFRESH = 9000;

// 标识auth token获取完毕
const uint32_t  FUNC_AUTH_TOKEN_VAILD = 9001;

// 标识基础商品查询完毕
const uint32_t  FUNC_QRY_SYMBOL_VAILD = 9002;

// 标识基础周期查询完毕
const uint32_t  FUNC_QRY_CYCLE_VAILD = 9003;

// 标识合约已经生成完毕
const uint32_t  FUNC_CONTRACT_GEN_VAILD = 9004;

// 标识 ws通道正常开启
const uint32_t  FUNC_WS_CHANNEL_VALID = 9005;


//error code define
#define COMMON_OK  0;
#define COMMON_FAILURE  -1;






#endif //CDEFINE_H

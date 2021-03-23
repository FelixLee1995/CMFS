/**
*  File: Init.h
*  Desc: 用于全局设置的函数， 仅调用一次
*  Created by felix on 2021/02/03
*/

#ifndef _INIT_H_
#define _INIT_H_
#pragma once


#include <iostream>
#include "core/CFlow.h"
#include "core/singleton.h"
#include "core/coredump.h"
#include "CConfig.hpp"
#include "CLog.h"



void InitEnv() 
{
    //SetCoredump();
    SetLogger();

    Singleton<CConfig>::Instance("./config/config.json");
    Singleton<CFlowManager>::Instance();

    auto flowManager = Singleton<CFlowManager>::GetInstance();
    flowManager->AddFlow(std::make_tuple(TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_SUB));
    flowManager->AddFlow(std::make_tuple(TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_UNSUB));
    flowManager->AddFlow(std::make_tuple(TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_SNAPSHOT_RTN));

    flowManager->AddFlow(std::make_tuple(TOPIC_USER_MANAGE, FUNC_REQ_USER_LOGIN));
    flowManager->AddFlow(std::make_tuple(TOPIC_USER_MANAGE, FUNC_REQ_USER_LOGOUT));
    flowManager->AddFlow(std::make_tuple(TOPIC_USER_MANAGE, FUNC_REQ_USER_DISCONNECT));





}

#endif
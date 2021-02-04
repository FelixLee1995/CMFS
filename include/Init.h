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
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "CConfig.hpp"
#include "spdlog/spdlog.h"

void SetLogger()
{
    auto logger = spdlog::daily_logger_mt("daily_logger", "logs/system.log");
    logger->set_level(spdlog::level::debug);

    logger->flush_on(spdlog::level::info);

    auto console_log_ptr = spdlog::stdout_color_mt("console");
    console_log_ptr->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%t] [%l] [%s:%#(%!)] %v");
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%t] [%l] [%s:%#(%!)] %v");

    spdlog::sinks_init_list sink_list = {logger->sinks().front(), console_log_ptr->sinks().front()};

    auto mylogger_ = std::make_shared<spdlog::logger>("my_logger", sink_list);

    spdlog::flush_every(std::chrono::seconds(1));

    spdlog::set_default_logger(mylogger_);

    SPDLOG_INFO("Server Start!");
}

void InitEnv() 
{
    SetLogger();
    Singleton<CConfig>::Instance("config/config.json");
    Singleton<CFlowManager>::Instance();

    auto flowManager = Singleton<CFlowManager>::GetInstance();
    flowManager->AddFlow(std::make_tuple(TOPIC_MARKET_PROCESS, FUNC_MARKET_RECV));
 

}

#endif
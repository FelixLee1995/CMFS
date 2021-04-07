/**
*  File: CLog.h
*  Desc: 日志包装
*  Created by felix on 2021/03/04
*/

#ifndef _LOG_H_
#define _LOG_H_
#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <iostream>
#include "CConfig.hpp"
#include "core/CFlow.h"
#include "core/coredump.h"
#include "core/singleton.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

namespace
{
void SetLogger()
{
    auto logger = spdlog::daily_logger_mt("mylog_daily", "logs/system.log");
    logger->set_level(spdlog::level::debug);

    //logger->flush_on(spdlog::level::info);

    auto console_log_ptr = spdlog::stdout_color_mt("console");
    console_log_ptr->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%t] [%l] [%s:%#(%!)] %v");

    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%t] [%l] [%s:%#(%!)] %v");

    //spdlog::sinks_init_list sink_list = {logger->sinks().front(), console_log_ptr->sinks().front()};

    spdlog::sinks_init_list sink_list = {logger->sinks().front(), console_log_ptr->sinks().front()};

    auto mylogger_ = std::make_shared<spdlog::logger>("mylogs", sink_list);

    mylogger_->set_level(spdlog::level::debug);

    spdlog::flush_every(std::chrono::seconds(1));

    spdlog::set_default_logger(mylogger_);

    spdlog::set_level(spdlog::level::debug);

    SPDLOG_DEBUG("Server Start!");
}
}  // namespace

#endif
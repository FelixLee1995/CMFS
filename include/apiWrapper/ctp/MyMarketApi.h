#ifndef _MY_MARKET_API_H_
#define _MY_MARKET_API_H_

#pragma once
#include <memory>
#include <iostream>
#include <cstring>
#include <vector>
#include <atomic>
#include "api/ctp/ThostFtdcMdApi.h"
#include "apiWrapper/ctp/MyMarketSpi.h"
#include "core/CFlow.h"
#include "nlohmann/json.hpp"


class MyMarketSpi;


class MyMarketApi {
    public:
        using Ptr = std::shared_ptr<MyMarketApi>;
        MyMarketApi(const nlohmann::json& configJson);
        ~MyMarketApi();

        void Init();
        //int ReqAuth();

        int ReqUserLogin(); 

        int ReqSubscribeMarketData(const std::string& instrumentID);
        int ReqSubscribeMarketData(const std::vector<std::string> instrVec);
        int ReqUnSubscribeMarketData(const std::string& instrumentID);

        void Dispose();

        bool GetConnectStatus(){ return m_front_connect_stautus.load();}
        bool GetLoginStatus(){return m_login_stautus;}

        const char* GetTradingDay();

    private:
        const int SYS_OK = 0;
        const int SYS_FAIL = -1;
        //reqid, increased
        int m_rid;
        std::string m_brokerid;
        std::string m_userid;
        std::string m_pwd;
        std::string m_front_addr;
        std::atomic<bool> m_front_connect_stautus; 
        std::atomic<bool> m_login_stautus; 

        CFlowManager::Sptr m_FlowManager;
        CThostFtdcMdApi * m_api;
        MyMarketSpi* m_spi;
        friend class MyMarketSpi;
};

#endif
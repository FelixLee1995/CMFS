/**
*  File: CCtpMarketAdapter.h
*  Desc: ctp行情接收适配器
*  Created by felix on 2021/03/16
*/


#ifndef _CTP_MARKET_ADAPTER_H_
#define _CTP_MARKET_ADAPTER_H_
#pragma once

#include "adapter/IAdapter.h"
#include "apiWrapper/ctp/MyMarketApi.h"

class CCtpMarketAdapter : public IMarketRecvAdapter
{
private:
    MyMarketApi::Ptr m_api;

public:
    explicit CCtpMarketAdapter(const nlohmann::json &config);
    void Init() override;
    void Stop() override;
    ~CCtpMarketAdapter();
};

#endif
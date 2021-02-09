/**
* File: CMarketPlugin.h
* Desc: 行情处理插件
* Created by felix on 2021/2/9
*/


#ifndef MARKET_PROCESS_PLUGIN_H
#define MARKET_PROCESS_PLUGIN_H
#pragma once

#include "interface/IPlugin.h"


class CMarketPlugin: public IPlugin
{
public:
    CMarketPlugin();
    void Init() override;
    void MsgHandler(const Msg &msg) override;
    void HandleSub(const Msg &msg);
    void HandleUnsub(const Msg &msg);
};


#endif
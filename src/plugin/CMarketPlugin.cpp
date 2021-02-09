//
// Created by felix on 2021/2/9.
//

#include "plugin/CMarketPlugin.h"

CMarketPlugin::CMarketPlugin(): IPlugin("MarketPlugin")
{}

void CMarketPlugin::Init()
{
    Subscribe(TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_SUB);
    Subscribe(TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_UNSUB);


}
void CMarketPlugin::MsgHandler(const Msg &msg)
{
    switch(msg.Header.FuncId) {
        case FUNC_REQ_MARKET_SUB: {
            HandleSub(msg);
            break;
        }
        case FUNC_REQ_MARKET_UNSUB: {
            HandleUnsub(msg);
            break;
        }
        default: {
            SPDLOG_ERROR("NO HANDLER FOR FUNCID: {}", msg.Header.FuncId);
            return;
        }
    }



}
void CMarketPlugin::HandleSub(const Msg &msg) {}
void CMarketPlugin::HandleUnsub(const Msg &msg) {}

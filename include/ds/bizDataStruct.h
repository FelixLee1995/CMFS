//
// Created by felix on 2020/9/22.
//

#ifndef FINANACE_MARKET_BIZDATASTRUCT_H
#define FINANACE_MARKET_BIZDATASTRUCT_H

#include "core/CBaseType.h"
#include <atomic>

enum MARKET_MSG_TYPE{
    MARKET = 0,
    ORDER,
    TRADE,
    SYMBOL,
    CYCLES,
};






struct env_status{
    std::atomic<bool> token_status_ ;
    std::atomic<bool> symbol_status ;
    std::atomic<bool> cycle_status ;
    std::atomic<bool> contact_status ;
    std::atomic<bool> ws_status ;

    env_status() {
        token_status_ = false;
        symbol_status = false;
        cycle_status = false;
        contact_status = false;
        ws_status = false;
    }
};


struct BaseCycle{
    uint32_t  Id;
    uint32_t  CycleDays;
};


struct BaseSymbol{
    uint32_t Id;
    FIELD_12 Symbol;
    FIELD_20 SymbolName;
    FIELD_10 ExchangeNo;
    FIELD_32  Remark;
    uint32_t Status;
};


struct Contract{
    BaseSymbol Symbol;
    BaseCycle Cycle;
};


enum MGS_SEND_TYPE {
    MSG_SEND_PRICE_DEPTH = 'A',
    MSG_SEND_HIDEEN_PRICE = 'B',
    MSG_SEND_HIGH_OPEN_LOW = 'C',
    MSG_SEND_DEAL = 'D',
    MSG_SEND_HEART_BEAT = 'E',
    MSG_SEND_CLEAR = 'F',
    MSG_SEND_FINANACE_HIGH_OPEN_LOW = 'G'
};


struct MarketBaseInfo{
    FIELD_20 ExchangeID;
    FIELD_16 ContractID;
    FIELD_12 Symbol;
    uint32_t  CycleDays;
    double High;
    double Low;
    double Open;
    double Close;
    double ReferenceRate;
    double AskRate;
    int AskVolume;
    int AskAllVolume;
    int Turnover;
    long long Timestamp;
    double ReferenceRate_Symbol;
    int AskVolume_Symbol;
};








//{
//"clientNo": "",
//"clientOrderId": "",
//"counterClientNo": "",
//"createAt": "",
//"cycleDays": 0,
//"direction": "",
//"exchangeNo": "",
//"id": 0,
//"lastTradeVolume": 0,
//"lockedAt": "",
//"lockedVolume": 0,
//"matchId": 0,
//"offset": "",
//"orderId": 0,
//"role": "",
//"symbol": "",
//"tradePrice": 0,
//"tradeVolume": 0,
//"updateAt": ""
//}


#endif //FINANACE_MARKET_BIZDATASTRUCT_H

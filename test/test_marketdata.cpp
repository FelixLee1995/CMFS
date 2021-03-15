#include <fstream>
#include <iostream>

#include "api/ctp/ThostFtdcUserApiStruct.h"

using namespace std;

int main()
{   
    CThostFtdcDepthMarketDataField marketdata;
    ifstream ifs("marketdata.dat", ios::binary| ios::in);
    ifs.read((char *)&marketdata, sizeof(CThostFtdcDepthMarketDataField));

    std::cout << "sizeof double is  "  <<  sizeof(double) << " , sizeof int is " << sizeof(int) << std::endl;

    std::cout << "last price is " << marketdata.LastPrice << std::endl;

    cout << "TradingDay: " << marketdata.TradingDay  << " , size: " << sizeof(marketdata.TradingDay) << endl;
    cout << "InstrumentID: " << marketdata.InstrumentID  << " , size: " << sizeof(marketdata.InstrumentID) << endl;
    cout << "ExchangeID: " << marketdata.ExchangeID  << " , size: " << sizeof(marketdata.ExchangeID) << endl;
    cout << "ExchangeInstID: " << marketdata.ExchangeInstID  << " , size: " << sizeof(marketdata.ExchangeInstID) << endl;

    cout << "LastPrice: " << hex << *((unsigned long long*)&marketdata.LastPrice) << " , size: " << sizeof(marketdata.LastPrice) << endl;
    cout << "PreSettlementPrice: " << hex << *((unsigned long long*)&marketdata.PreSettlementPrice) <<  " , size: " << sizeof(marketdata.PreSettlementPrice) << endl;
    cout << "PreClosePrice: " << hex << *((unsigned long long*)&marketdata.PreClosePrice) << " , size: " << sizeof(marketdata.PreClosePrice) << endl;
    cout << "PreOpenInterest: " << hex << *((unsigned long long*)&marketdata.PreOpenInterest) << " , size: " << sizeof(marketdata.PreOpenInterest) << endl;
    cout << "OpenPrice: " << hex << *((unsigned long long*)&marketdata.OpenPrice) << " , size: " << sizeof(marketdata.OpenPrice) << endl;
    cout << "HighestPrice: " << hex << *((unsigned long long*)&marketdata.HighestPrice) << " , size: " << sizeof(marketdata.HighestPrice) << endl;
    cout << "LowestPrice: " << hex << *((unsigned long long*)&marketdata.LowestPrice) << " , size: " << sizeof(marketdata.LowestPrice) << endl;
    cout << "Volume: " << hex << *((int*)&marketdata.Volume) << " , size: " << sizeof(marketdata.Volume) << endl;
    cout << "Turnover: " << hex << *((unsigned long long*)&marketdata.Turnover) << " , size: " << sizeof(marketdata.Turnover) << endl;
    cout << "OpenInterest: " << hex << *((unsigned long long*)&marketdata.OpenInterest) << " , size: " << sizeof(marketdata.OpenInterest) << endl;
    cout << "ClosePrice: " << hex << *((unsigned long long*)&marketdata.ClosePrice) << " , size: " << sizeof(marketdata.ClosePrice) << endl;
    cout << "SettlementPrice: " << hex << *((unsigned long long*)&marketdata.SettlementPrice) << " , size: " << sizeof(marketdata.SettlementPrice) << endl;
    cout << "UpperLimitPrice: " << hex << *((unsigned long long*)&marketdata.UpperLimitPrice) << " , size: " << sizeof(marketdata.UpperLimitPrice) << endl;
    cout << "LowerLimitPrice: " << hex << *((unsigned long long*)&marketdata.LowerLimitPrice) << " , size: " << sizeof(marketdata.LowerLimitPrice) << endl;
    cout << "PreDelta: " << hex << *((unsigned long long*)&marketdata.PreDelta) << " , size: " << sizeof(marketdata.PreDelta) << endl;
    cout << "CurrDelta: " << hex << *((unsigned long long*)&marketdata.CurrDelta) << " , size: " << sizeof(marketdata.CurrDelta) << endl;
    cout << "UpdateTime: " << marketdata.UpdateTime << " , size: " << sizeof(marketdata.UpdateTime) << endl;
    cout << "UpdateMillisec: " << hex << *((int*)&marketdata.UpdateMillisec) << " , size: " << sizeof(marketdata.UpdateMillisec) << endl;

    cout << "BidPrice1: " << hex << *((unsigned long long*)&marketdata.BidPrice1) << " , size: " << sizeof(marketdata.BidPrice1) << endl;
    cout << "BidVolume1: " << hex << *((int*)&marketdata.BidVolume1) << " , size: " << sizeof(marketdata.BidVolume1) << endl;
    cout << "AskPrice1: " << hex << *((unsigned long long*)&marketdata.AskPrice1) << " , size: " << sizeof(marketdata.AskPrice1) << endl;
    cout << "AskVolume1: " << hex << *((int*)&marketdata.AskVolume1) << " , size: " << sizeof(marketdata.AskVolume1) << endl;

    cout << "BidPrice2: " << hex << *((unsigned long long*)&marketdata.BidPrice2) << " , size: " << sizeof(marketdata.BidPrice1) << endl;
    cout << "BidVolume2: " << hex << *((int*)&marketdata.BidVolume2) << " , size: " << sizeof(marketdata.BidVolume1) << endl;
    cout << "AskPrice2: " << hex << *((unsigned long long*)&marketdata.AskPrice2) << " , size: " << sizeof(marketdata.AskPrice1) << endl;
    cout << "AskVolume2: " << hex << *((int*)&marketdata.AskVolume2) << " , size: " << sizeof(marketdata.AskVolume1) << endl;

    cout << "BidPrice3: " << hex << *((unsigned long long*)&marketdata.BidPrice3) << " , size: " << sizeof(marketdata.BidPrice1) << endl;
    cout << "BidVolume3: " << hex << *((int*)&marketdata.BidVolume3) << " , size: " << sizeof(marketdata.BidVolume1) << endl;
    cout << "AskPrice3: " << hex << *((unsigned long long*)&marketdata.AskPrice3) << " , size: " << sizeof(marketdata.AskPrice1) << endl;
    cout << "AskVolume3: " << hex << *((int*)&marketdata.AskVolume3) << " , size: " << sizeof(marketdata.AskVolume1) << endl;

    cout << "BidPrice4: " << hex << *((unsigned long long*)&marketdata.BidPrice4) << " , size: " << sizeof(marketdata.BidPrice1) << endl;
    cout << "BidVolume4: " << hex << *((int*)&marketdata.BidVolume4) << " , size: " << sizeof(marketdata.BidVolume1) << endl;
    cout << "AskPrice4: " << hex << *((unsigned long long*)&marketdata.AskPrice4) << " , size: " << sizeof(marketdata.AskPrice1) << endl;
    cout << "AskVolume4: " << hex << *((int*)&marketdata.AskVolume4) << " , size: " << sizeof(marketdata.AskVolume1) << endl;

    cout << "BidPrice5: " << hex << *((unsigned long long*)&marketdata.BidPrice5) << " , size: " << sizeof(marketdata.BidPrice5) << endl;
    cout << "BidVolume5: " << hex << *((int*)&marketdata.BidVolume5) << " , size: " << sizeof(marketdata.BidVolume5) << endl;
    cout << "AskPrice5: " << hex << *((unsigned long long*)&marketdata.AskPrice5) << " , size: " << sizeof(marketdata.AskPrice5) << endl;
    cout << "AskVolume5: " << hex << *((int*)&marketdata.AskVolume5) << " , size: " << sizeof(marketdata.AskVolume5) << endl;



    cout << "AveragePrice: " << hex << *((unsigned long long*)&marketdata.AveragePrice) << " , size: " << sizeof(marketdata.AveragePrice) << endl;
    cout << "ActionDay: " << marketdata.ActionDay << " , size: " << sizeof(marketdata.ActionDay) << endl;




    return 0;
}

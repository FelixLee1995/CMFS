#ifndef _TEST_SUIT_MD_H_
#define _TEST_SUIT_MD_H_

#pragma once

#include <iostream>
#include <gtest/gtest.h>
#include "core/plantformtools.h"
#include "apiWrapper/ctp/MyMarketApi.h"


const std::string brokerid = "9999";
const std::string userid = "123616";
const std::string pwd = "nanase4ever";
//const std::string marketfrontAddr = "tcp://180.168.146.187:10110";
const std::string marketfrontAddr = "tcp://127.0.0.1:17001";
const std::string appid = "simnow_client_test";
const std::string authcode = "0000000000000000";

// const std::string brokerid = "2071";
// const std::string userid = "80186354";
// const std::string pwd = "asdf852963";
// const std::string marketfrontAddr = "tcp://180.169.50.131:42213";
// //const std::string marketfrontAddr = "tcp://127.0.0.1:17001";
// const std::string appid = "client_LEE121_T1";
// const std::string authcode = "3WWJKJ";



#ifdef ENVGTJA
const std::string brokerid = "2071";
const std::string userid = "00004703";
const std::string pwd = "888888";
const std::string frontAddr = "tcp://180.169.50.131:42205";
const std::string appid = "simnow_client_test";
const std::string authcode = "0000000000000000"; 
#endif



class MdApiTest: public testing::Test {

    protected:

    virtual void SetUp() override {
        p_api = new MyMarketApi(brokerid, userid, pwd, marketfrontAddr);

        p_api->Init();
        CommonSleep(2000);
        EXPECT_EQ(p_api->GetConnectStatus(), true);
  
        p_api->ReqUserLogin();
        CommonSleep(2000);

        // EXPECT_EQ(p_api->ReqUnSubscribeMarketData("a2105"), 0);        
        // usleep(5000000);
        EXPECT_EQ(p_api->GetLoginStatus(), true);
    };

    virtual void TearDown() override {
        delete p_api;
        p_api = nullptr;
    }

    MyMarketApi * p_api;
};


#endif  //_TEST_SUIT_MD_H_
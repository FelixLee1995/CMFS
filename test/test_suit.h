#ifndef TESTSUIT_H
#define TESTSUIT_H
#pragma once

#include <iostream>
#include <gtest/gtest.h>

#include "api/ctp/ThostFtdcUserApiStruct.h"
#include "core/cache/CSSCache.h"






class CacheTest: public testing::Test {

    protected:

    virtual void SetUp() override {
        cache = new CSSCache<CThostFtdcDepthMarketDataField, 200>();

    };

    virtual void TearDown() override {
        
    }

    CSSCache<CThostFtdcDepthMarketDataField, 200> * cache;
};



#endif




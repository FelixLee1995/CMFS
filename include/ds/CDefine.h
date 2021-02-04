//
// Created by felix on 2020/9/21.
//  用于定义基础宏
//



#ifndef CDEFINE_H
#define CDEFINE_H

#include <cstdint>


using FUNCID_TYPE = uint16_t;
using TOPICID_TYPE = uint16_t;

/// topic define
const TOPICID_TYPE TOPIC_MARKET_PROCESS = 0x1100;



const FUNCID_TYPE FUNC_REQ_MARKET_SUB = 0x2001;
const FUNCID_TYPE FUNC_MARKET_RECV = 0x2002;



//error code define
#define COMMON_OK  0;
#define COMMON_FAILURE  -1;






#endif //CDEFINE_H

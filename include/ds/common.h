/**
*  File: common.h
*  Desc: define common data struct
*  Created by felix on 2020/09/04
*/

#pragma once
#include <iostream>



#define MSG_PACK_MAX_LENGTH  1024


using byte = unsigned char;
using UserSessionIdType = int32_t;
using SessionIdType = int32_t;


struct MsgHeader {
    uint8_t Count;  ///表示消息体中有几个连续的消息
    uint16_t TopicId;
    uint16_t FuncId;
    uint16_t ContentLen;
    SessionIdType SessionId;
};



struct Msg{
    MsgHeader Header;
    char Pack[MSG_PACK_MAX_LENGTH];

//    Msg(): Header(){
//        std::memset(Pack, 0, sizeof(Pack));
//    }
//
//    Msg(Msg && ori) noexcept : Header(ori.Header) {
//        std::memcpy(Pack, ori.Pack, sizeof(Pack));
//    }
//
//
//
//    Msg& operator = (Msg && ori)  noexcept {
//        std::swap(Header.ContentLen, ori.Header.ContentLen);
//        std::swap(Header.FuncId, ori.Header.FuncId);
//        std::swap(Header.TopicId, ori.Header.TopicId);
//        std::memcpy(Pack, ori.Pack, sizeof(Pack));
//    }


};
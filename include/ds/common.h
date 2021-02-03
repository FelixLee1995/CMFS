/**
*  File: common.h
*  Desc: define common data struct
*  Created by felix on 2020/09/04
*/

#pragma once
#include <iostream>



#define MSG_PACK_MAX_LENGTH  1024



struct MsgHeader {
    uint32_t TopicId;
    uint32_t FuncId;
    uint32_t ContentLen;
};



struct Msg{
    MsgHeader Header;
    char Pack[1024];

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
/**
*  File: CByteOrder.h
*  Desc: 字节序转换相关操作
*  Created by felix on 2020/09/08
*/
#pragma once

namespace felix
{
    void swapByteOrder(unsigned short &us)
    {
        us = (us >> 8) |
             (us << 8);
    }
    void swapByteOrder(unsigned int &ui)
    {
        ui = (ui >> 24) |
             ((ui << 8) & 0x00FF0000) |
             ((ui >> 8) & 0x0000FF00) |
             (ui << 24);
    }
    void swapByteOrder(unsigned long long &ull)
    {
        ull = (ull >> 56) |
              ((ull << 40) & 0x00FF000000000000) |
              ((ull << 24) & 0x0000FF0000000000) |
              ((ull << 8) & 0x000000FF00000000) |
              ((ull >> 8) & 0x00000000FF000000) |
              ((ull >> 24) & 0x0000000000FF0000) |
              ((ull >> 40) & 0x000000000000FF00) |
              (ull << 56);
    }

}; // namespace felix

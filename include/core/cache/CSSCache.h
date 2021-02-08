/**
*  File: CSSCache.h
*  Desc: SingleSimple Cache / 单一结构简单缓存
*  Use for: 单一结构内容的快速缓存， 一般数据量不大， 对性能要求高的查找
*  Created by felix on 2021/02/04
*/
#ifndef CSSCACHE_H
#define CSSCACHE_H
#pragma once



template<typename Field, int size>
class CSSCache
{
   private:
    void * m_Data;

   public:
    CSSCache():
    m_Data(nullptr)
    {
        m_Data = new Field[size];
    };
    

};





#endif
/**
*  File: nocopyable.h
*  Desc: 无法复制的类， 用于继承获得该特性
*  Created by felix on 2020/09/01
*/


#pragma once

class Nocopyable {

protected:

    /**
     * @brief 保留默认构造函数
     */
    Nocopyable() = default;

    /**
     * @brief 保留默认析构函数
     */
    ~Nocopyable() = default;

    /**
     * @brief 拷贝构造函数(禁用)
     */
    Nocopyable(const Nocopyable& T) = delete;


    /**
     * @brief 赋值函数(禁用)
     */
    Nocopyable& operator = (const Nocopyable& ) = delete;


};
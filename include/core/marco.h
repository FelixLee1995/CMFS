/**
*  File: marco.h
*  Desc: 自定义的宏
*  Created by felix on 2021/04/12
*/

#ifndef _MY_MACRO_H_
#define _MY_MACRO_H_
#pragma once

#if defined __GNUC__ || defined __llvm__
/// LIKCLY 宏的封装, 告诉编译器优化,条件大概率成立
#   define MY_LIKELY(x)       __builtin_expect(!!(x), 1)
/// LIKCLY 宏的封装, 告诉编译器优化,条件大概率不成立
#   define MY_UNLIKELY(x)     __builtin_expect(!!(x), 0)
#else
#   define MY_LIKELY(x)      (x)
#   define MY_UNLIKELY(x)      (x)
#endif


#endif
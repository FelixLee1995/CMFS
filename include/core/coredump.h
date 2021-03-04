/**
*  File: coredump.h
*  Desc: 设置系统产生coredump文件大小
*  Created by felix on 2020/09/01
*/

#pragma once

#include <iostream>

#ifdef linux
#include <sys/resource.h>
#endif

#define CORE_SIZE 1024 * 1024 * 10000

int static SetCoredump(){

#ifdef linux
    struct rlimit rlmt;
    if (getrlimit(RLIMIT_CORE, &rlmt) == -1) {
        return -1;
    }

    rlmt.rlim_cur = (rlim_t) CORE_SIZE;
    rlmt.rlim_max = (rlim_t) CORE_SIZE;

    if (setrlimit(RLIMIT_CORE, &rlmt) == -1) {
        printf("setrlimit fail!\n");
        return -1;
    } else {
        printf("setrlimit success!\n");
    }
#endif
    return 0;
}


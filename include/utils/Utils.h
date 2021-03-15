//
// Created by lf729 on 2020/9/18.
//

#ifndef _UTILS_H
#define _UTILS_H
#include <string>

#include "nettrans.h"

#if defined(__linux__)
    #include <sys/time.h>
#endif 

#define BZERO(STRUCT)    \
std::memset(&STRUCT, 0, sizeof(STRUCT));  \


namespace {
std::string GetFormatTimeStr9()
{
//    time_t timep;
//    time (&timep);
//    char tmp[9];
//    strftime(tmp, sizeof(tmp), "%02H:%02M:%02S",localtime(&timep));
//    return std::string(tmp);

    time_t nowtime=time(NULL);
    struct tm *l=localtime(&nowtime);
    char buf[128];
    snprintf(buf,sizeof(buf),"%02d:%02d:%02d",l->tm_hour,l->tm_min,l->tm_sec);
    std::string s(buf);
    return s;
}

#define MY_HTONLL(x)    (((long long int)x & 0x00000000000000ff) << 7*8)|\
							 (((long long int)x & 0x000000000000ff00) << 5*8)|\
							 (((long long int)x & 0x0000000000ff0000) << 3*8)|\
							 (((long long int)x & 0x00000000ff000000) << 1*8)|\
							 (((long long int)x & 0x000000ff00000000) >> 1*8)|\
							 (((long long int)x & 0x0000ff0000000000) >> 3*8)|\
							 (((long long int)x & 0x00ff000000000000) >> 5*8)|\
							 (((long long int)x & 0xff00000000000000) >> 7*8)


}




#endif //_UTILS_H

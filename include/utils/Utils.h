//
// Created by lf729 on 2020/9/18.
//

#ifndef _UTILS_H
#define _UTILS_H
#include <string>

using namespace std;


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
}





#endif //_UTILS_H

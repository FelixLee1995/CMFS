//
// Created by lf729 on 2020/9/18.
//

#ifndef _UTILS_H
#define _UTILS_H
#include <string>

using namespace std;


#define BZERO(STRUCT)    \
std::memset(&STRUCT, 0, sizeof(STRUCT));  \



std::string  static GetFormatTimeStr9()
{
    time_t timep;
    time (&timep);
    char tmp[9];
    strftime(tmp, sizeof(tmp), "%02H:%02M:%02S",localtime(&timep));
    return std::string(tmp);
}




#endif //_UTILS_H

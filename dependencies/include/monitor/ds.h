//
// Created by felix lee on 2020/10/9.
//

#ifndef MONITORINTEGRATION_DS_H
#define MONITORINTEGRATION_DS_H
#include <string>

namespace mi{

    const int MI_OK = 0;
    const int MI_FAILURE = -1;


    enum MonitorSeverity {
        Normal = 0,
        Error = 1,
        Fatal = 2
    };


    enum HealthStatus {
        UP = 0,
        UNKNOWN,
        OUT_OF_SERVICE,
        DOWN,
    };

    enum HealthSeverityLevel {
        NORMAL = 0,
        ERR = 1,
        FATAL = 2,
    };


    struct HealthTotal {
        int StatusCode;
        std::string Desc;
    };


    struct HealthDetail {
        char Key[32];
        char Desc[128];
        int StatusCode;
        int Level;
    };
}

#endif //MONITORINTEGRATION_DS_H

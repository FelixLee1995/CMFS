//
// Created by felix lee on 2020/10/9.
//
#pragma  once

#ifndef MONITORINTEGRATION_IMONITORAPP_H
#define MONITORINTEGRATION_IMONITORAPP_H

#if defined(_MSC_VER) || defined(_WIN32) || defined(_WIN64)

#ifdef MYDLL_EXPORTS
#define MYDLL_API __declspec(dllexport)
#else
#define MYDLL_API __declspec(dllimport)
#endif


#else
#define MYDLL_API
#endif


#include <string>
#include <functional>
#include "ds.h"

namespace mi{


    class MYDLL_API IMonitorApp{
    public:
        virtual void RegisterIndicatorFunc(const char *key, const std::function<HealthDetail(const std::string& key)> &f) = 0;
        virtual void MonitorOut(const char * issue, const char* desc, MonitorSeverity ) = 0;

        virtual int RegisterHealthDetailItem(const char *key) = 0;

        virtual int PostSuccessDetail(const char *key, const std::string & desc = "STATUS SUCCESS") = 0;

        virtual int PostFailDetail(const char *key, const char * desc) = 0;
    };



    MYDLL_API mi::IMonitorApp* factory_func();

    extern "C" MYDLL_API mi::IMonitorApp* g_monitor_;

}




#endif //MONITORINTEGRATION_IMONITORAPP_H

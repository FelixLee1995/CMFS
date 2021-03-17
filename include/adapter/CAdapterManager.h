/**
*  File: CAdapterManager.h
*  Desc: 行情适配器管理模块， 用来启动相应的行情适配器
*  Created by felix on 2021/03/16
*/

#ifndef _ADAPTER_MANAGER_H_
#define _ADAPTER_MANAGER_H_
#pragma once

#include <vector>
#include "CConfig.hpp"
#include "core/baseheader.h"
#include "adapter/IAdapter.h"

class CAdapterManager
{

    private:

    CConfig::Ptr m_ConfigPtr;
    std::vector<IMarketRecvAdapter*> m_AdapterVec;
    void Initdapter(const std::string& type, const nlohmann::json& configJson);

    public:
    CAdapterManager();
    void Init();

};

#endif
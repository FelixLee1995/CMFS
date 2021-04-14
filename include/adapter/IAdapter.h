//
// Created by felix on 2021/3/9.
//

#ifndef CMFS_IADAPTER_H
#define CMFS_IADAPTER_H

#pragma once

#include <string>
#include "CConfig.hpp"
#include "core/CFlow.h"
#include "CLog.h"
#include "core/singleton.h"


class IMarketRecvAdapter
{
private:
    std::string m_Name;
    CConfig::Ptr m_ConfigSptr;

protected:
    CFlowManager::Sptr m_FlowManagerSptr;

public:
    explicit IMarketRecvAdapter(std::string  adapterName):
    m_Name(std::move(adapterName))
    {
        m_ConfigSptr.reset(Singleton<CConfig>::GetInstance());
        m_FlowManagerSptr.reset(Singleton<CFlowManager>::GetInstance());
        assert(m_ConfigSptr);
        assert(m_FlowManagerSptr);
    }

    ~IMarketRecvAdapter()
    {
        SPDLOG_INFO("adapter [{}] deconstruction!", m_Name);
        //todo  行情适配器停止的逻辑
    }

    virtual void Init() = 0;
    virtual void Stop() = 0;

};





#endif  //CMFS_IADAPTER_H

//
// Created by felix on 2021/3/9.
//

#ifndef CMFS_IADAPTER_H
#define CMFS_IADAPTER_H

#pragma once

#include <string>
#include "CConfig.hpp"
#include "core/CFlow.h"


#define PUB_BIZ_MSG_TO_PLUGIN(FLOWMANAGER_PTR, TOPIC_ID, FUNC_ID, SESSION_ID, CONTENT, LEN, CNT) \
    do                                                       \
    {                                                        \
        Msg msg{};                                           \
        memset(&msg, 0, sizeof(msg));                        \
        msg.Header.Count = CNT;                              \
        msg.Header.TopicId = TOPIC_ID;                       \
        msg.Header.FuncId = FUNC_ID;                         \
        msg.Header.SessionId = SESSION_ID;                   \
        msg.Header.ContentLen = LEN;                         \
        memcpy(msg.Pack, CONTENT, LEN);                      \
        FLOWMANAGER_PTR->PublishMsg(msg);                         \
    }while(0);                                               \


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

/**
* File: IPlugin.h
* Desc: 插件基类
* Created by felix on 2021/2/9
*/

#ifndef IPLUGIN_H
#define IPLUGIN_H
#pragma once

#include <string>
#include <utility>
#include "CLog.h"
#include "core/CFlow.h"
#include "interface/IQueue.h"
#include "CConfig.hpp"
#include "core/singleton.h"
#include "core/BlockingConcurrentQueue.h"
#include "ds/common.h"

class IPlugin
{
private:
    std::string m_Name;
    CConfig::Ptr m_ConfigSptr;
    BlockingConcurrentQueue<Msg, 1024> m_MsgQueue;


public:
    explicit IPlugin(std::string  pluginName):
          m_Name(std::move(pluginName)), m_MsgQueue([this](auto && pH1) { MsgHandler(std::forward<decltype(pH1)>(pH1)); })
    {
        m_ConfigSptr.reset(Singleton<CConfig>::GetInstance());
        assert(m_ConfigSptr);
        SPDLOG_INFO("plugin[{}] started", m_Name);
    }

    ~IPlugin()
    {
        SPDLOG_INFO("plugin[{}] deconstruction!", m_Name);
        //todo  插件停止的逻辑
    }

    virtual void Init() = 0;

//    virtual void OnRequest() = 0;
//    virtual void OnRecvMsg(const Msg&) = 0;
    virtual void Subscribe(uint32_t topicId, uint32_t funcId) {
        auto* flowManager = Singleton<CFlowManager>::GetInstance();
        flowManager->Subscribe(std::make_tuple(topicId, funcId), [&](const Msg& msg){
            auto inQueueRet = m_MsgQueue.EnQueue(msg);
            if (!inQueueRet) {
                SPDLOG_ERROR("Failed to EnQueue, plugin[{}]", m_Name);
            }
        });
    }

    //�Զ�����Ϣ����
    virtual void MsgHandler(const Msg&) = 0;


};


#endif
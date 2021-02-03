/**
*  File: CFlow.h
*  Desc: 流， 可提供订阅发布， dump、反演
*  Created by felix on 2020/09/04
*/

#pragma once
#include <functional>
#include <atomic>
#include <map>
#include <tuple>
#include <ds/common.h>
#include <core/OBEvent.h>
#include <core/ConcurrentQueue.h>
#include <core/BlockingConcurrentQueue.h>

#include <ds/CDefine.h>

enum {
    TOPIC_NOT_EXISTS = -1,
    UNKOWN_ERROR = -2
};


// tuple<topcid(channel) ,  funcid>
using funcid_t = std::tuple<uint32_t, uint32_t>;


//todo transfer to template class
class CMsgFlow:public OBEvent<std::function<void(const Msg&)>>{

public:

    CMsgFlow(): queue_(std::bind(&CMsgFlow::Proc, this, std::placeholders::_1)){}

    void start() {

    }

    void dump() {}

    void react() {}

    ~CMsgFlow() {
        queue_.Stop();
    }

    int InQueue(const Msg &msg) {
        return queue_.EnQueue(msg) ? 1 : 0;
    };

    void Proc(const Msg& msg) {
        auto cnt = NotifyAll(msg);
    };

private:
    std::atomic<uint64_t> seq_no_;
    std::atomic<uint64_t> cur_idx_;

    BlockingConcurrentQueue<Msg, 1024> queue_;
};


class CFlowManager{

    public:

        void AddFlow(funcid_t topicId) {
            auto iter = m_flows.find(topicId);
            if (iter != m_flows.end()) {
                std::cout << "flow already exists, topicId: " << std::get<0>(topicId)  << " funcId: " << std::get<1>(topicId) << std::endl;
            }
            else {
                auto msg_flow_ptr = std::make_shared<CMsgFlow>();
                msg_flow_ptr->start();
                m_flows[topicId] = msg_flow_ptr;
            }
        }

        std::shared_ptr<CMsgFlow> GetFlow(const funcid_t& topicId) {
            auto iter = m_flows.find(topicId);
            if (iter != m_flows.end()) {
                return iter->second;
            }
            return nullptr;
        }



        int Subscribe(const funcid_t& topicId, std::function<void(const Msg&)> handleFunc) {
            auto iter = m_flows.find(topicId);
            if (iter!=m_flows.end()) {
                return iter->second->Register(std::forward<std::function<void(const Msg&)>>(handleFunc));
            }
            return TOPIC_NOT_EXISTS;
        }



        int PublishMsg(const Msg& msg) {

            auto iter = m_flows.find(std::make_tuple(msg.Header.TopicId, msg.Header.FuncId));

            if (iter != m_flows.end()) {
                return iter->second->InQueue(msg);
            }
            return TOPIC_NOT_EXISTS;
        }


        int UnSubscribe(funcid_t) {
            return 0;
        }


    private:

    std::map<funcid_t, std::shared_ptr<CMsgFlow>> m_flows;

};
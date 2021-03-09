//
// Created by felix on 2020/9/24.
//

#ifndef FINANACE_MARKET_CONCURRENTQUEUE_H
#define FINANACE_MARKET_CONCURRENTQUEUE_H

#include <interface/IQueue.h>
#include <functional>
#include "core/queue/concurrentqueue.h"

template <typename T, int MAX_CAPICITY>
class ConcurrentQueue: public IQueue<T>{
public:
    explicit ConcurrentQueue(std::function<void(const T&)>proc_func) {
        cb_ = proc_func;
        isRunning = true;
        proc_thread_ptr = std::make_shared<std::thread>(&ConcurrentQueue::ProcThread, this);
    }
    ~ ConcurrentQueue()
    {
        std::cout << "ConcurrentQueue deconstruct!" << std::endl;
        Stop();
    }


    void Stop() override{
        isRunning = false;
        proc_thread_ptr->join();
        proc_thread_ptr.reset();
    }

    void ProcThread() {
        T data;
        while (isRunning) {
            if (queue_.try_dequeue(data))
            {
                cb_(std::move(data));
            }
            //std::cout << "lockfree queue threadid : " << this_thread::get_id() << std::endl;
        }
    }


    bool  EnQueue(const T& data) override {
        return queue_.enqueue(data);
    }
    //todo Dequeue thread owner
    bool DeQueue(T &) override {
        return true;
    };
private:
    moodycamel::ConcurrentQueue<T> queue_;
    std::function<void(const T&)> cb_;
    std::shared_ptr<std::thread> proc_thread_ptr;
    bool isRunning = false;
};



#endif //FINANACE_MARKET_CONCURRENTQUEUE_H

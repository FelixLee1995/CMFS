//
// Created by felix on 2020/9/22.
//

#ifndef FINANACE_MARKET_LOCKFREERINGQUEUE_H
#define FINANACE_MARKET_LOCKFREERINGQUEUE_H

#include "interface/IQueue.h"
#include "core/ringbuffer.hpp"
#include <functional>
#include <thread>
#include <memory>

template <typename T, int MAX_CAPICITY>
class LockFreeRingQueue: public IQueue<T>{
public:
    explicit LockFreeRingQueue(std::function<void(const T&)>proc_func) {
        cb_ = proc_func;
        isRunning = true;
        proc_thread_ptr = std::make_shared<std::thread>(&LockFreeRingQueue::ProcThread, this);
    }
    ~ LockFreeRingQueue()
    {
        std::cout << "LockFreeRingQueue deconstruct!" << std::endl;
        Stop();
    }


    void Stop() override{
        isRunning = false;
        proc_thread_ptr->join();
        proc_thread_ptr.reset();
    }

    void ProcThread(){
        while(isRunning) {
            T data;
            while(queue.remove(data)) {
                cb_(std::move(data));
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                //std::cout << "lockfree queue threadid : " << this_thread::get_id() << std::endl;
            }
        }
    }


    bool  EnQueue(const T& data) override {
        return queue.insert(data);
    }
    //todo Dequeue thread owner
    bool DeQueue(T &) override {
        return true;
    };
private:
    Ringbuffer<T, MAX_CAPICITY> queue;
    std::function<void(const T&)> cb_;
    std::shared_ptr<std::thread> proc_thread_ptr;
    bool isRunning = false;
};




#endif //FINANACE_MARKET_LOCKFREERINGQUEUE_H

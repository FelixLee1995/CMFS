//
// Created by felix on 2020/9/24.
//

#ifndef FINANACE_MARKET_BLOCKINGCONCURRENTQUEUE_H
#define FINANACE_MARKET_BLOCKINGCONCURRENTQUEUE_H


#include <interface/IQueue.h>
#include <functional>
#include "moodycamelqueue/blockingconcurrentqueue.h"

template <typename T, int MAX_CAPICITY, bool isCpuPin = false>
class BlockingConcurrentQueue: public IQueue<T>{
public:
    explicit BlockingConcurrentQueue(std::function<void(const T&)>proc_func) {
        cb_ = proc_func;
        isRunning = true;
        proc_thread_ptr = std::make_shared<std::thread>(&BlockingConcurrentQueue::ProcThread, this);
    }
    ~ BlockingConcurrentQueue()
    {
        std::cout << "BlockingConcurrentQueue deconstruct!" << std::endl;
        Stop();
    }


    void Stop() override{
        isRunning = false;
        proc_thread_ptr->join();
        proc_thread_ptr.reset();
    }

    void ProcThread() {

//        if (isCpuPin) {
//            SetThreadAffinityMask(proc_thread_ptr->native_handle(), 0x0001);
//        }

        T data;
        while (isRunning) {
            if (queue_.wait_dequeue_timed(data, std::chrono::milliseconds(10)))
                cb_(std::move(data));
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
    moodycamel::BlockingConcurrentQueue<T> queue_;
    std::function<void(const T&)> cb_;
    std::shared_ptr<std::thread> proc_thread_ptr;
    bool isRunning = false;
};




#endif //FINANACE_MARKET_BLOCKINGCONCURRENTQUEUE_H

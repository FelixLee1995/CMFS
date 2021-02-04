//
// Created by lf729 on 2020/9/22.
//

#ifndef FINANACE_MARKET_IQUEUE_H
#define FINANACE_MARKET_IQUEUE_H

#include "ds/common.h"

template <typename T>
class IQueue {
public:
    virtual bool EnQueue(const T&) = 0;
    virtual bool DeQueue(T &) = 0;
    virtual void Stop() = 0;
};

#endif //FINANACE_MARKET_IQUEUE_H

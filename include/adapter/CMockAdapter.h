//
// Created by felix on 2021/3/9.
//

#ifndef CMFS_CMOCKADAPTER_H
#define CMFS_CMOCKADAPTER_H

#pragma  once

#include <memory>
#include <thread>
#include <atomic>

#include "CLog.h"
#include "adapter/IAdapter.h"


class CMockAdapter: public IMarketRecvAdapter
{
private:
    std::shared_ptr<std::thread> m_SendThread;
    std::atomic_bool m_IfWorking;

public:
    CMockAdapter();
    void Init() override;
    void Stop() override;
    void SendThread();
};

#endif  //CMFS_CMOCKADAPTER_H

/**
*  File: CServer.h
*  Desc: TcpServer类， 使用了asio异步网络框架
*  Created by felix on 2021/02/08
*/


#ifndef TCP_SERVER_H
#define TCP_SERVER_H
#pragma once

#include <atomic>
#include <map>
#include "asio.hpp"
#include "CTcpSession.h"
#include "core/CFlow.h"

class CTcpSession;
class CTcpChatroom;

class CTcpServer
{
    private:
        std::atomic_uint16_t m_SessionIdx;
        std::atomic_bool m_IsWorking;
        std::size_t m_MaxOnlineUsers;
        int m_Port;
        std::map<int, std::shared_ptr<CTcpSession>> m_SessionMap;   /// 维护tcp会话映射
        asio::ip::tcp::acceptor m_Acceptor;
        std::shared_ptr<CTcpChatroom> m_ChatroomPtr;
        CFlowManager::Sptr m_FlowManager;
                
    public:
        using Sptr = std::shared_ptr<CTcpServer>;
        CTcpServer(std::size_t maxOnlineUsers, int port, asio::io_context& ctx);
        ~CTcpServer();
        void Start();
        void Stop();
        void DoAccept();
        int PubBizMsg(Msg msg);
};





#endif
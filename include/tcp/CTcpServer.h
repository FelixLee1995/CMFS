/**
*  File: CServer.h
*  Desc: TcpServer类， 使用了asio异步网络框架
*  Created by felix on 2021/02/08
*/


#ifndef TCP_SERVER_H
#define TCP_SERVER_H
#pragma once

#include <atomic>
#include "asio.hpp"
#include "CTcpSession.h"

class CTcpServer
{
    private:
        std::atomic_bool m_IsWorking;
        std::size_t m_MaxOnlineUsers;
        int m_Port;
        tcp::acceptor m_Acceptor;
        CTcpChatroom m_Chatroom;
        asio::io_context m_IoContext;
                
    public:
        CTcpServer(std::size_t maxOnlineUsers, int port);
        ~CTcpServer();
        void Start();
        void Stop();
        void DoAccept();
};





#endif
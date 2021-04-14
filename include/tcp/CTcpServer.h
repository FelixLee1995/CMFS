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
#include <mutex>
#include "asio.hpp"
#include "CTcpSession.h"
#include "core/CFlow.h"
#include "api/ctp_ext/ctp_ftdc_proto.h"

class CTcpSession;
class CTcpChatroom;


using namespace ctp_ftd;

    

class CTcpServer
{
    private:
        std::atomic<int32_t> m_SessionIdx;
        std::atomic_bool m_IsWorking;
        std::size_t m_MaxOnlineUsers;
        int m_Port;
        std::map<int, std::shared_ptr<CTcpSession>> m_SessionMap;   /// 维护tcp会话映射
        asio::ip::tcp::acceptor m_Acceptor;
        std::shared_ptr<CTcpChatroom> m_ChatroomPtr;
        CFlowManager::Sptr m_FlowManager;
        std::mutex m_Mutex;
    public:
        using Sptr = std::shared_ptr<CTcpServer>;
        CTcpServer(std::size_t maxOnlineUsers, int port, asio::io_context& ctx);
        ~CTcpServer();
        void Start();
        void Stop();
        void DoAccept();
        int PubBizMsg(Msg msg);

        int SendRspFtdc(TOPICID_TYPE topicId, SessionIdType sessionId, ctp_ftd::ftdc_tid_type ftdc_tid, 
            const char * fieldPtr, size_t fieldLen, ctp_ftd::ftdc_fid_type ftdc_fid, int errorId, const char* errorMsg);
        
        int SendRtnFtdc(TOPICID_TYPE topicId, SessionIdType sessionId, ctp_ftd::ftdc_tid_type ftdc_tid,
            const char * fieldPtr, size_t fieldLen, ctp_ftd::ftdc_fid_type ftdc_fid);

        int SendMultiRtnFtdc(TOPICID_TYPE topicId, SessionIdType sessionId, ctp_ftd::ftdc_tid_type ftdc_tid,
            const char *fieldPtr, size_t fieldLen, ctp_ftd::ftdc_fid_type ftdc_fid, size_t cnt);

        int SendMsg(char * data, unsigned int len, SessionIdType sessionId, TOPICID_TYPE topicId);
};





#endif
#include "tcp/CTcpServer.h"
#include "spdlog/spdlog.h"
#include "core/singleton.h"

CTcpServer::CTcpServer(std::size_t maxOnlineUsers, int port, asio::io_context& ctx):
    m_SessionIdx(0),
    m_MaxOnlineUsers(maxOnlineUsers),
    m_Port(port),
    m_IsWorking(false),
    m_Acceptor(ctx, tcp::endpoint(tcp::v4(), port)),
    m_SessionMap(),
    m_Chatroom()

{
    m_FlowManager.reset(Singleton<CFlowManager>::GetInstance());

    DoAccept();
    SPDLOG_INFO("after doAccept");
}

CTcpServer::~CTcpServer()
{
    //todo tcpserver 析构
}


void CTcpServer::Start()
{
    //m_IoContext.run();
    SPDLOG_DEBUG("after ioc run");
}

void CTcpServer::DoAccept()
{
    m_Acceptor.async_accept([this](asio::error_code ec, tcp::socket socket) {
        if (!ec)
        {
            SPDLOG_INFO("socketfd is {}", socket.native_handle());
            m_SessionIdx == (++m_SessionIdx)% UINT16_MAX;               //idx不断循环自增， 避免和socketfd无法对应
            auto session = std::make_shared<CTcpSession>(std::move(socket), m_Chatroom, m_SessionIdx);
            session->Start();

            m_SessionMap.emplace(m_SessionIdx, session);
        }
        DoAccept();
    });
}
int CTcpServer::PubBizMsg(Msg msg)
{

    m_FlowManager->PublishMsg(msg);

    return 0;
}

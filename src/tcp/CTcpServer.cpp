#include "tcp/CTcpServer.h"
#include "spdlog/spdlog.h"

CTcpServer::CTcpServer(std::size_t maxOnlineUsers, int port):
    m_MaxOnlineUsers(maxOnlineUsers),
    m_Port(port),
    m_IsWorking(false),
    m_IoContext(),
    m_Acceptor(m_IoContext, tcp::endpoint(tcp::v4(), port))
{
    DoAccept();
    SPDLOG_INFO("after doAccept");
}

CTcpServer::~CTcpServer()
{

}


void CTcpServer::Start()
{
    m_IoContext.run();
    SPDLOG_DEBUG("after ioc run");
}

void CTcpServer::DoAccept()
{
    m_Acceptor.async_accept([this](asio::error_code ec, tcp::socket socket) {
        if (!ec)
        {
            std::make_shared<CTcpSession>(std::move(socket), m_Chatroom)->Start();
        }
        DoAccept();
    });
}

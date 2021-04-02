#include "tcp/CTcpServer.h"
#include "spdlog/spdlog.h"
#include "core/singleton.h"
#include "ds/common.h"

CTcpServer::CTcpServer(std::size_t maxOnlineUsers, int port, asio::io_context& ctx):
    m_SessionIdx(0),
    m_IsWorking(false),
    m_MaxOnlineUsers(maxOnlineUsers),
    m_Port(port),
    m_SessionMap(),
    m_Acceptor(ctx, tcp::endpoint(tcp::v4(), port))
    
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
            m_SessionIdx = (++m_SessionIdx)% INT32_MAX;               //idx不断循环自增， 避免和socketfd无法对应
            auto session = std::make_shared<CTcpSession>(std::move(socket), m_SessionIdx);
            session->Start();
            {
                std::lock_guard<std::mutex> guard(m_Mutex);
                m_SessionMap.emplace(m_SessionIdx, session);  /// TODO  断线时候，  从sessionMap中删除
            }
        }
        DoAccept();
    });
}
int CTcpServer::PubBizMsg(Msg msg)
{

    m_FlowManager->PublishMsg(msg);

    return 0;
}


int CTcpServer::SendMsg(char * data, unsigned int len, SessionIdType sessionId, TOPICID_TYPE topicId)
{   
    std::lock_guard<std::mutex> guard(m_Mutex);

    auto session_iter = m_SessionMap.find(sessionId);

    if (session_iter == m_SessionMap.end())
    {
        SPDLOG_ERROR("sendMsg to session[{}] failed", sessionId);
        return COMMON_FAILURE;
    }




    MessageWrapper msg;

    if (len > msg.max_body_length)
    {
        SPDLOG_ERROR("msg length oversize");
        return COMMON_FAILURE;
    }

    memcpy(msg.data_, data, len);
    msg.body_length_ = len - 4;

    auto session_ptr = session_iter->second;
    session_ptr->deliver(msg);

    return COMMON_OK;
}
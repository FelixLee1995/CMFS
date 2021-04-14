//
// Created by felix on 2021/2/9.
//

#include "tcp/CTcpSession.h"
#include "CLog.h"
#include "core/singleton.h"

CRevcBuffer::CRevcBuffer()
{
    m_Data = new unsigned char[1024];
}

CRevcBuffer::~CRevcBuffer()
{
    delete[] m_Data;
}

unsigned char *CRevcBuffer::Data()
{
    return m_Data;
}

size_t CRevcBuffer::Length()
{
    return m_Len;
}

MessageWrapper::MessageWrapper() : body_length_(0)
{
    memset(data_, 0, header_length + max_body_length);
}

const unsigned char *MessageWrapper::data() const
{
    return data_;
}

unsigned char *MessageWrapper::data()
{
    return data_;
}

std::size_t MessageWrapper::length() const
{
    return header_length + body_length_;
}

const unsigned char *MessageWrapper::body() const
{
    return data_ + header_length;
}

unsigned char *MessageWrapper::body()
{
    return data_ + header_length;
}

std::size_t MessageWrapper::body_length() const
{
    return body_length_;
}

void MessageWrapper::body_length(std::size_t new_length)
{
    body_length_ = new_length;
    if (body_length_ > max_body_length)
        body_length_ = max_body_length;
}

bool MessageWrapper::decode_header()
{
    memcpy(&body_length_, data_ + 3, 1);
    memcpy(&body_length_ + 1, data_ + 2, 1);

    body_length_ -= 1;
    if (body_length_ > max_body_length)
    {
        body_length_ = 5;
    }
    return true;
}

void MessageWrapper::encode_header()
{
    unsigned char header[header_length + 1] = "";
    //std::sprintf(header, "%4d", static_cast<int>(body_length_));
    std::memcpy(data_, header, header_length);
}

CTcpSession::CTcpSession(tcp::socket socket, SessionIdType sessionId)
    : m_Socket(std::move(socket)), m_RecvBuffer(), m_SessionId(sessionId)
{
    m_Server.reset(Singleton<CTcpServer>::GetInstance());
    m_FlowManager.reset(Singleton<CFlowManager>::GetInstance());
    assert(m_Server);
    assert(m_FlowManager);
}

void CTcpSession::Start()
{
    DoReadHeader();
}

void CTcpSession::DoReadHeader()
{
    auto self(shared_from_this());
    asio::async_read(m_Socket, asio::buffer(m_Msg.data(), MessageWrapper::header_length),
        [this, self](asio::error_code ec, std::size_t /*length*/) {
            if (ec)
            {
                /// ec 表示读取错误
                SPDLOG_ERROR("asio read failed: {}", ec.message());
                /// 发送消息到业务插件。　处理客户下线
                m_FlowManager->PubBizMsg2Plugin(TOPIC_USER_MANAGE, FUNC_REQ_USER_DISCONNECT, m_SessionId, nullptr, 0, 1);
                
                return;
            }

            ftd_header *header = (ftd_header *)m_Msg.data();
            header->ftd_content_len = ntohs(header->ftd_content_len);
            if (header->ftd_content_len > 0)
            {
                m_Msg.body_length(header->ftd_content_len);
            }
            else
            {
                m_Msg.body_length(0);
            }
            m_Msg.ext_len_ = header->ftd_ext_len;
            do_read_body();
        });
}

void CTcpSession::do_read_body()
{
    auto self(shared_from_this());
    asio::async_read(m_Socket, asio::buffer(m_Msg.body(), m_Msg.body_length() + m_Msg.ext_len_),
        [this, self](asio::error_code ec, std::size_t /*length*/) {
            if (!ec)
            {
                do
                {
                    /// 只有ext体， 无具体业务包时， 为心跳消息， 直接返回心跳消息即可
                    if (m_Msg.body_length() == 0)
                    {
                        MessageWrapper msg;
                        memcpy(msg.data_, bHeartBeat, sizeof(bHeartBeat));
                        msg.body_length_ = sizeof(bHeartBeat) - 4;
                        deliver(msg);
                        break;
                    }

                    /// 解ftdc包
                    auto offset = sizeof(ftdc_crpheader) + m_Msg.ext_len_;
                    ftdc_header *ftdcheader = (ftdc_header *)(m_Msg.body() + offset);
                    auto cnt = ntohs(ftdcheader->ftdc_field_count);
                    uint32_t topicID = ntohl(ftdcheader->ftdc_topic_id);
                    uint16_t contenLen = ntohs(ftdcheader->ftdc_content_len);

                    auto contenLenCheck = contenLen < MSG_PACK_MAX_LENGTH;
                    if (contenLenCheck == false)
                    {
                        SPDLOG_ERROR("contentLen oversize, ftdc_topicid[{}]", topicID);
                        /// TODO 处理contenlen 超长的问题
                        return;
                    }

                    char *content = reinterpret_cast<char *>(ftdcheader) + sizeof(ftdc_header);

                    /// 根据topciID 进行业务处理
                    switch (topicID)
                    {
                        case ftdc_fid_SOPT_Req_Init:
                        {
                            MessageWrapper msg;
                            memcpy(msg.data_, bInitRspOpt, sizeof(bInitRspOpt));
                            msg.body_length_ = sizeof(bInitRspOpt) - 4;

                            deliver(msg);
                            break;
                        }
                        case ftdc_fid_ReqInit:
                        {
                            MessageWrapper msg;
                            memcpy(msg.data_, bInitRsp, sizeof(bInitRsp));
                            msg.body_length_ = sizeof(bInitRsp) - 4;
                            deliver(msg);
                            break;
                        }
                        case ftdc_fid_FirstReq:
                        {
                            MessageWrapper msg;
                            memcpy(msg.data_, bFirstRsp, sizeof(bFirstRsp));
                            msg.body_length_ = sizeof(bFirstRsp) - 4;
                            deliver(msg);
                            break;
                        }
                        /// 用户登录请求
                        case ftdc_fid_ReqLogin:
                        {
                            m_FlowManager->PubBizMsg2Plugin(TOPIC_USER_MANAGE, FUNC_REQ_USER_LOGIN, m_SessionId, content, contenLen, cnt);
                            break;
                        }

                        /// 用户登出请求
                        case ftdc_fid_ReqUserLogout:
                        {
                            m_FlowManager->PubBizMsg2Plugin(TOPIC_USER_MANAGE, FUNC_REQ_USER_LOGOUT, m_SessionId, content, contenLen, cnt);
                            break;
                        }
                        /// 行情订阅请求
                        case ftdc_fid_ReqSub:
                        {
                            m_FlowManager->PubBizMsg2Plugin(TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_SUB, m_SessionId, content, contenLen, cnt);
                            break;
                        }

                        /// 取消订阅请求
                        case ftdc_fid_ReqUnsub:
                        {
                            m_FlowManager->PubBizMsg2Plugin(TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_UNSUB, m_SessionId,
                                content, contenLen, cnt);
                            break;
                        }
                        default:
                            SPDLOG_ERROR("unsupported requestType: {}", topicID);
                            break;
                    }

                } while (0);

                DoReadHeader();
            }
        });
}

void CTcpSession::deliver(const MessageWrapper &msg)
{

    RWMutex::WriteLock guard(m_Mutex);

    //bool write_in_progress = !m_MsgQueue.size_approx;
    m_MsgQueue.enqueue(msg);

    DoWrite();
}

void CTcpSession::DoWrite()
{
    auto self(shared_from_this());
    MessageWrapper msg;
    auto ret = m_MsgQueue.try_dequeue(msg);
    if (!ret)
    {
        return;
    }
    asio::async_write(
        m_Socket, asio::buffer(msg.data(), msg.length()), [this, self](asio::error_code ec, std::size_t /*length*/) {
            if (!ec)
            {
                DoWrite();
            }
        });
}
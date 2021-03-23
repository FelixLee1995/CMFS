//
// Created by felix on 2021/2/9.
//

#include "tcp/CTcpSession.h"
#include "core/singleton.h"
#include "CLog.h"


#define PUB_BIZ_MSG_TO_PLUGIN(SERVER_SPTR, TOPIC_ID, FUNC_ID, SESSION_ID, CONTENT, LEN, CNT) \
    do                                                       \
    {                                                        \
        Msg msg{};                                           \
        memset(&msg, 0, sizeof(msg));                        \
        msg.Header.Count = CNT;                              \
        msg.Header.TopicId = TOPIC_ID;                       \
        msg.Header.FuncId = FUNC_ID;                         \
        msg.Header.SessionId = SESSION_ID;                   \
        msg.Header.ContentLen = LEN;                         \
        memcpy(msg.Pack, CONTENT, LEN);                      \
        SERVER_SPTR->PubBizMsg(msg);                         \
    }while(0);                                               \

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

void CTcpChatroom::join(communitor_ptr participant)
{
    // m_Communicators.insert(participant);
    // for (auto msg: m_MsgQueue)
    //     participant->deliver(msg);
}

void CTcpChatroom::leave(communitor_ptr participant)
{
    m_Communicators.erase(participant);
}




CTcpSession::CTcpSession(tcp::socket socket, std::shared_ptr<CTcpChatroom> chatroom, int32_t sessionId)
    : m_Socket(std::move(socket)), m_ChatroomPtr(chatroom), m_RecvBuffer(), m_SessionId(sessionId)
{
    m_Server.reset(Singleton<CTcpServer>::GetInstance());
}

void CTcpSession::Start()
{
    m_ChatroomPtr->join(shared_from_this());
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

                

                PUB_BIZ_MSG_TO_PLUGIN(
                                m_Server, TOPIC_USER_MANAGE, FUNC_REQ_USER_DISCONNECT, m_SessionId, nullptr, 0, 1);
                /// TODO　发送到业务层。　通知客户下线
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

                    auto offset = sizeof(ftdc_crpheader) + m_Msg.ext_len_;
                    ftdc_header *ftdcheader = (ftdc_header *)(m_Msg.body() + offset);
                    auto cnt = ntohs(ftdcheader->ftdc_field_count);
                    uint32_t topicID = ntohl(ftdcheader->ftdc_topic_id);
                    uint16_t seriesID = ntohs(ftdcheader->ftdc_seq_series);
                    uint32_t seq_no = ntohl(ftdcheader->ftdc_seq_no);
                    uint32_t req_id = ntohl(ftdcheader->ftdc_req_id);
                    uint16_t contenLen = ntohs(ftdcheader->ftdc_content_len);


                    SPDLOG_DEBUG("topicid is {}, seriesID is {}, seq_no is {}, reqid_is {}", topicID, seriesID, seq_no, req_id);

                    auto contenLenCheck = contenLen < MSG_PACK_MAX_LENGTH;
                    if (contenLenCheck == false)
                    {
                        SPDLOG_ERROR("contentLen oversize, ftdc_topicid[{}]", topicID);
                        /// TODO 处理contenlen 超长的问题
                    }

                    char *content = reinterpret_cast<char *>(ftdcheader) + sizeof(ftdc_header);

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
                        case ftdc_fid_ReqLogin:
                        {
                            PUB_BIZ_MSG_TO_PLUGIN(m_Server, TOPIC_USER_MANAGE, FUNC_REQ_USER_LOGIN, m_SessionId, content,
                                contenLen, cnt);
                            break;
                        }

                         case ftdc_fid_ReqUserLogout:
                        {
                            PUB_BIZ_MSG_TO_PLUGIN(m_Server, TOPIC_USER_MANAGE, FUNC_REQ_USER_LOGOUT, m_SessionId, content,
                                contenLen, cnt);
                            break;
                        }
                        case ftdc_fid_ReqSub:
                        {
                            PUB_BIZ_MSG_TO_PLUGIN(
                                m_Server, TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_SUB, m_SessionId, content, contenLen, cnt);
                            break;
                        }
                        case ftdc_fid_ReqUnsub:
                        {
                            PUB_BIZ_MSG_TO_PLUGIN(
                                m_Server, TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_UNSUB, m_SessionId, content, contenLen, cnt);
                            break;
                        }
                        default:
                            printf("content is %s\n", content);
                            break;
                    }

                } while (0);

                DoReadHeader();
            }
            else
            {
                m_ChatroomPtr->leave(shared_from_this());
            }
        });
}

void CTcpSession::deliver(const MessageWrapper &msg)
{
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
    asio::async_write(m_Socket, asio::buffer(msg.data(), msg.length()),
        [this, self](asio::error_code ec, std::size_t /*length*/) {
            if (!ec)
            {
                DoWrite();
            }
            else
            {
                m_ChatroomPtr->leave(shared_from_this());
            }
        });
}
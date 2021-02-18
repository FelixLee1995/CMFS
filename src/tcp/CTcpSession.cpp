//
// Created by felix on 2021/2/9.
//

#include "tcp/CTcpSession.h"
#include "core/singleton.h"
#include "spdlog/spdlog.h"



#define PUB_BIZ_MSG_TO_PLUGIN(SERVER_SPTR, TOPIC_ID, FUNC_ID, SESSION_ID, CONTENT, LEN) \
    do                                                           \
    {                                                        \
        Msg msg{};                                           \
        memset(&msg, 0, sizeof(msg));                            \
        msg.Header.TopicId = TOPIC_USER_MANAGE;              \
        msg.Header.FuncId = FUNC_REQ_USER_LOGIN;             \
        msg.Header.SessionId = SESSION_ID;                   \
        msg.Header.ContentLen = LEN;                         \
        memcpy(msg.Pack, CONTENT, LEN);                      \
        SERVER_SPTR->PubBizMsg(msg);                         \
    }while(0); \




void CTcpChatroom::join(communitor_ptr participant)
{
    m_Communicators.insert(participant);
    for (auto msg: m_MsgQueue)
        participant->deliver(msg);
}

void CTcpChatroom::leave(communitor_ptr participant)
{
    m_Communicators.erase(participant);
}




CTcpSession::CTcpSession(tcp::socket socket, std::shared_ptr<CTcpChatroom> chatroom, uint16_t sessionId)
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

            std::cout << "parse header success,  headerLen " << MessageWrapper::header_length << ", bodyLen "
                      << m_Msg.body_length() + m_Msg.ext_len_ << std::endl;
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
                auto offset = sizeof(ftdc_crpheader) + m_Msg.ext_len_;
                ftdc_header *ftdcheader = (ftdc_header *)(m_Msg.body() + offset);
                uint32_t topicID = ntohl(ftdcheader->ftdc_topic_id);
                uint16_t seriesID = ntohs(ftdcheader->ftdc_seq_series);
                uint32_t seq_no = ntohl(ftdcheader->ftdc_seq_no);
                uint32_t req_id = ntohl(ftdcheader->ftdc_req_id);
                uint16_t contenLen = ntohs(ftdcheader->ftdc_content_len);

                std::cout << "topicid is " << topicID << " seriesID is " << seriesID << " seq_no is " << seq_no
                          << " req_id is " << req_id << std::endl;

                auto contenLenCheck = contenLen < MSG_PACK_MAX_LENGTH;
                if (contenLenCheck == false) {
                    SPDLOG_ERROR("contentLen oversize, ftdc_topicid[{}]", topicID);
                }

                char *content = reinterpret_cast<char *>(ftdcheader) + sizeof(ftdc_header);

                switch (topicID)
                {
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
                        // PUB_BIZ_MSG_TO_PLUGIN(m_Server, TOPIC_USER_MANAGE, FUNC_REQ_USER_LOGIN, m_SessionId, content,
                        //     ftdcheader->ftdc_content_len);
                        Msg msg{};
                        memset(&msg, 0, sizeof(msg));
                        msg.Header.TopicId = TOPIC_USER_MANAGE;
                        msg.Header.FuncId = FUNC_REQ_USER_LOGIN;
                        msg.Header.SessionId = m_SessionId;
                        msg.Header.ContentLen = contenLen;
                        memcpy(msg.Pack, content, contenLen);
                        m_Server->PubBizMsg(msg);

                        break;
                    }
                    case ftdc_fid_ReqSub:
                    {
                        PUB_BIZ_MSG_TO_PLUGIN(m_Server, TOPIC_MARKET_PROCESS, FUNC_REQ_MARKET_SUB, m_SessionId, content,
                                              ftdcheader->ftdc_content_len);
                        break;
                    }
                    default:
                        printf("content is %s\n", content);
                        break;
                }

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
    bool write_in_progress = !m_MsgQueue.empty();
    m_MsgQueue.push_back(msg);
    if (!write_in_progress)
    {
        std::cout << "before do write , threadid is " << std::this_thread::get_id() << std::endl;

        DoWrite();
    }
}

void CTcpSession::DoWrite()
{
    auto self(shared_from_this());
    asio::async_write(m_Socket, asio::buffer(m_MsgQueue.front().data(), m_MsgQueue.front().length()),
        [this, self](asio::error_code ec, std::size_t /*length*/) {
            if (!ec)
            {
                m_MsgQueue.pop_front();
                if (!m_MsgQueue.empty())
                {
                    DoWrite();
                }
            }
            else
            {
                m_ChatroomPtr->leave(shared_from_this());
            }
        });
}
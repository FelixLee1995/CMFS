#include "tcp/CTcpServer.h"
#include "core/singleton.h"
#include "ds/common.h"
#include "spdlog/spdlog.h"

CTcpServer::CTcpServer(std::size_t maxOnlineUsers, int port, asio::io_context &ctx)
    : m_SessionIdx(0), m_IsWorking(false), m_MaxOnlineUsers(maxOnlineUsers), m_Port(port), m_SessionMap(),
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
            m_SessionIdx = (++m_SessionIdx) % INT32_MAX;  //idx不断循环自增， 避免和socketfd无法对应
            auto session = std::make_shared<CTcpSession>(std::move(socket), m_SessionIdx);
            session->Start();
            {
                RWMutex::WriteLock guard(m_Mutex);
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

int CTcpServer::SendRspFtdc(TOPICID_TYPE topicId, SessionIdType sessionId, ctp_ftd::ftdc_tid_type ftdc_tid,
    const char *fieldPtr, size_t fieldLen, ctp_ftd::ftdc_fid_type ftdc_fid, int errorId, const char *errorMsg)
{
    char oriData[2048] = {0};
    unsigned int oriLen = 2 * sizeof(ftdc_field_header) + sizeof(CThostFtdcRspInfoField) + fieldLen + sizeof(ftdc_header);
    char encodedData[2048] = {0};
    unsigned int encodedLen = 0;
    char *data = oriData;
    auto ftdc_header_s = (ftdc_header *)data;
    ftdc_header_s->ftdc_version = ftdc_version_cur;
    ftdc_header_s->ftdc_chain = ftd_chain_last;
    ftdc_header_s->ftdc_topic_id = htonl(ftdc_tid);
    ftdc_header_s->ftdc_seq_series = htons(ftdc_sid_none);
    ftdc_header_s->ftdc_seq_no = htonl(0);
    ftdc_header_s->ftdc_field_count = htons(2);
    ftdc_header_s->ftdc_content_len = htons(2 * sizeof(ftdc_field_header) + sizeof(CThostFtdcRspInfoField) + fieldLen);
    ftdc_header_s->ftdc_req_id = htonl(0);
    data += sizeof(ftdc_header);
    auto header1 = (ftdc_field_header *)data;
    header1->field_id = htons(ftdc_fid_RspInfoField);
    header1->field_len = htons(sizeof(CThostFtdcRspInfoField));
    data += sizeof(ftdc_field_header);
    auto rspInfoField = (CThostFtdcRspInfoField *)data;
    rspInfoField->ErrorID = htonl(errorId);
    strcpy(rspInfoField->ErrorMsg, errorMsg);
    data += sizeof(CThostFtdcRspInfoField);
    auto header2 = (ftdc_field_header *)data;
    header2->field_id = htons(ftdc_fid);
    header2->field_len = htons(fieldLen);
    data += sizeof(ftdc_field_header);
    memcpy(data, fieldPtr, fieldLen);
    EncodeZero(oriData, oriLen, encodedData + sizeof(ftd_header) + sizeof(ftdc_crpheader), &encodedLen);
    auto ftd_header_s = (ftd_header *)encodedData;
    ftd_header_s->ftd_type = ftd_type_compressed;
    ftd_header_s->ftd_ext_len = 0;
    ftd_header_s->ftd_content_len = htons(encodedLen + sizeof(ftdc_crpheader));
    auto ftdc_crpheader_s = (ftdc_crpheader *)(encodedData + sizeof(ftd_header));
    ftdc_crpheader_s->protocol = ftdc_crp_protocol;
    ftdc_crpheader_s->method = ftdc_cmp_compresszero;
    auto senLen = encodedLen + sizeof(ftd_header) + sizeof(ftdc_crpheader);
    SendMsg(encodedData, senLen, sessionId, topicId);
}

int CTcpServer::SendRtnFtdc(TOPICID_TYPE topicId, SessionIdType sessionId, ctp_ftd::ftdc_tid_type ftdc_tid,
    const char *fieldPtr, size_t fieldLen, ctp_ftd::ftdc_fid_type ftdc_fid)
{
    char oriData[2048] = {0};
    unsigned int oriLen = sizeof(ftdc_field_header) + fieldLen + sizeof(ftdc_header);
    char encodedData[2048] = {0};
    unsigned int encodedLen = 0;
    char *data = oriData;
    auto ftdc_header_s = (ftdc_header *)data;
    ftdc_header_s->ftdc_version = ftdc_version_cur;
    ftdc_header_s->ftdc_chain = ftd_chain_last;
    ftdc_header_s->ftdc_topic_id = htonl(ftdc_tid);
    ftdc_header_s->ftdc_seq_series = htons(ftdc_sid_none);
    ftdc_header_s->ftdc_seq_no = htonl(0);
    ftdc_header_s->ftdc_field_count = htons(1);
    ftdc_header_s->ftdc_content_len = htons(sizeof(ftdc_field_header) + fieldLen);
    ftdc_header_s->ftdc_req_id = htonl(0);
    data += sizeof(ftdc_header);
    auto header1 = (ftdc_field_header *)data;
    header1->field_id = htons(ftdc_fid);
    header1->field_len = htons(fieldLen);
    data += sizeof(ftdc_field_header);
    memcpy(data, fieldPtr, fieldLen);
    EncodeZero(oriData, oriLen, encodedData + sizeof(ftd_header) + sizeof(ftdc_crpheader), &encodedLen);
    auto ftd_header_s = (ftd_header *)encodedData;
    ftd_header_s->ftd_type = ftd_type_compressed;
    ftd_header_s->ftd_ext_len = 0;
    ftd_header_s->ftd_content_len = htons(encodedLen + sizeof(ftdc_crpheader));
    auto ftdc_crpheader_s = (ftdc_crpheader *)(encodedData + sizeof(ftd_header));
    ftdc_crpheader_s->protocol = ftdc_crp_protocol;
    ftdc_crpheader_s->method = ftdc_cmp_compresszero;
    auto senLen = encodedLen + sizeof(ftd_header) + sizeof(ftdc_crpheader);
    SendMsg(encodedData, senLen, sessionId, topicId);
}

int CTcpServer::SendMultiRtnFtdc(TOPICID_TYPE topicId, SessionIdType sessionId, ctp_ftd::ftdc_tid_type ftdc_tid,
    const char *fieldPtr, size_t fieldLen, ctp_ftd::ftdc_fid_type ftdc_fid, size_t cnt)
{
    char oriData[4096] = {0};
    unsigned int oriLen = cnt * (sizeof(ftdc_field_header) + fieldLen) + sizeof(ftdc_header);
    char encodedData[4096] = {0};
    unsigned int encodedLen = 0;
    char *data = oriData;
    auto ftdc_header_s = (ftdc_header *)data;
    ftdc_header_s->ftdc_version = ftdc_version_cur;
    ftdc_header_s->ftdc_chain = ftd_chain_last;
    ftdc_header_s->ftdc_topic_id = htonl(ftdc_tid);
    ftdc_header_s->ftdc_seq_series = htons(ftdc_sid_none);
    ftdc_header_s->ftdc_seq_no = htonl(0);
    ftdc_header_s->ftdc_field_count = htons(cnt);
    ftdc_header_s->ftdc_content_len = htons(cnt * (sizeof(ftdc_field_header) + fieldLen));
    ftdc_header_s->ftdc_req_id = htonl(0);
    data += sizeof(ftdc_header);
    for (auto i = 0; i < cnt; ++i)
    {
        auto header1 = (ftdc_field_header *)data;
        header1->field_id = htons(ftdc_fid);
        header1->field_len = htons(fieldLen);
        data += sizeof(ftdc_field_header);
        memcpy(data, fieldPtr + i * fieldLen, fieldLen);
        data += fieldLen;
    }
    EncodeZero(oriData, oriLen, encodedData + sizeof(ftd_header) + sizeof(ftdc_crpheader), &encodedLen);
    auto ftd_header_s = (ftd_header *)encodedData;
    ftd_header_s->ftd_type = ftd_type_compressed;
    ftd_header_s->ftd_ext_len = 0;
    ftd_header_s->ftd_content_len = htons(encodedLen + sizeof(ftdc_crpheader));
    auto ftdc_crpheader_s = (ftdc_crpheader *)(encodedData + sizeof(ftd_header));
    ftdc_crpheader_s->protocol = ftdc_crp_protocol;
    ftdc_crpheader_s->method = ftdc_cmp_compresszero;
    auto senLen = encodedLen + sizeof(ftd_header) + sizeof(ftdc_crpheader);
    SendMsg(encodedData, senLen, sessionId, topicId);
}

int CTcpServer::SendMsg(char *data, unsigned int len, SessionIdType sessionId, TOPICID_TYPE topicId)
{
    RWMutex::ReadLock guard(m_Mutex);

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
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

#define TCP_SEND_RSPINFO(TOPICID, TCP_SERVER_PTR, SESSIONID, FTDC_TID, SPEC_RSP_INFO_FIELD_TYPE, SPEC_RSP_INFO_FIELD_PTR,  SPEC_RSP_FID, ERROR_ID, ERROR_MSG)     \
do {  \
    char oriData[2048] = {0};  \
    unsigned int oriLen = 2*sizeof(ftdc_field_header) + sizeof(CThostFtdcRspInfoField) + sizeof(SPEC_RSP_INFO_FIELD_TYPE) + sizeof(ftdc_header);  \
    char encodedData[2048] = {0};  \
    unsigned int encodedLen = 0;  \
    char* data =  oriData; \
    auto  ftdc_header_s = (ftdc_header *)data;    \
    ftdc_header_s->ftdc_version = ftdc_version_cur;  \
    ftdc_header_s->ftdc_chain = ftd_chain_last; \
    ftdc_header_s->ftdc_topic_id = htonl(FTDC_TID);  \
    ftdc_header_s->ftdc_seq_series = htons(ftdc_sid_none);  \
    ftdc_header_s->ftdc_seq_no = htonl(0);   \
    ftdc_header_s->ftdc_field_count = htons(2);   \
    ftdc_header_s->ftdc_content_len = htons(2*sizeof(ftdc_field_header) + sizeof(CThostFtdcRspInfoField) + sizeof(SPEC_RSP_INFO_FIELD_TYPE)); \
    ftdc_header_s->ftdc_req_id = htonl(0);  \
    data += sizeof(ftdc_header);   \
    auto header1 = (ftdc_field_header*) data; \
    header1->field_id = htons(ftdc_fid_RspInfoField); \
    header1->field_len = htons(sizeof(CThostFtdcRspInfoField)); \
    data += sizeof(ftdc_field_header); \
    auto rspInfoField = (CThostFtdcRspInfoField*) data; \
    rspInfoField->ErrorID = htonl(ERROR_ID);   \
    strcpy(rspInfoField->ErrorMsg, ERROR_MSG);  \
    data += sizeof(CThostFtdcRspInfoField);  \
    auto header2 = (ftdc_field_header *)data; \
    header2->field_id = htons(SPEC_RSP_FID); \
    header2->field_len = htons(sizeof(SPEC_RSP_INFO_FIELD_TYPE)); \
    data += sizeof(ftdc_field_header); \
    auto specRspField = (SPEC_RSP_INFO_FIELD_TYPE *)data; \
    memcpy(specRspField, SPEC_RSP_INFO_FIELD_PTR, sizeof(SPEC_RSP_INFO_FIELD_TYPE)); \
    EncodeZero(oriData, oriLen, encodedData +  sizeof(ftd_header) + sizeof(ftdc_crpheader), &encodedLen); \
    auto ftd_header_s = (ftd_header*) encodedData; \
    ftd_header_s->ftd_type = ftd_type_compressed; \
    ftd_header_s->ftd_ext_len = 0; \
    ftd_header_s->ftd_content_len = htons(encodedLen + sizeof(ftdc_crpheader)); \
    auto ftdc_crpheader_s = (ftdc_crpheader*)(encodedData + sizeof(ftd_header)); \
    ftdc_crpheader_s->protocol = ftdc_crp_protocol; \
    ftdc_crpheader_s->method = ftdc_cmp_compresszero; \
    auto senLen = encodedLen + sizeof(ftd_header) + sizeof(ftdc_crpheader); \
    TCP_SERVER_PTR->SendMsg(encodedData, senLen, SESSIONID, TOPICID);  \
}while(0) \
    


#define TCP_SEND_RTNINFO(TOPICID, TCP_SERVER_PTR, SESSIONID, FTDC_TID, SPEC_RSP_INFO_FIELD_TYPE, SPEC_RSP_INFO_FIELD_PTR,  SPEC_RSP_FID)     \
do {  \
    char oriData[2048] = {0};  \
    unsigned int oriLen = sizeof(ftdc_field_header) + sizeof(SPEC_RSP_INFO_FIELD_TYPE) + sizeof(ftdc_header);  \
    char encodedData[2048] = {0};  \
    unsigned int encodedLen = 0;  \
    char* data =  oriData; \
    auto  ftdc_header_s = (ftdc_header *)data;    \
    ftdc_header_s->ftdc_version = ftdc_version_cur;  \
    ftdc_header_s->ftdc_chain = ftd_chain_last; \
    ftdc_header_s->ftdc_topic_id = htonl(FTDC_TID);  \
    ftdc_header_s->ftdc_seq_series = htons(ftdc_sid_none);  \
    ftdc_header_s->ftdc_seq_no = htonl(0);   \
    ftdc_header_s->ftdc_field_count = htons(1);   \
    ftdc_header_s->ftdc_content_len = htons(sizeof(ftdc_field_header) + sizeof(SPEC_RSP_INFO_FIELD_TYPE)); \
    ftdc_header_s->ftdc_req_id = htonl(0);  \
    data += sizeof(ftdc_header);   \
    auto header1 = (ftdc_field_header *)data; \
    header1->field_id = htons(SPEC_RSP_FID); \
    header1->field_len = htons(sizeof(SPEC_RSP_INFO_FIELD_TYPE)); \
    data += sizeof(ftdc_field_header); \
    auto specRspField = (SPEC_RSP_INFO_FIELD_TYPE *)data; \
    memcpy(specRspField, SPEC_RSP_INFO_FIELD_PTR, sizeof(SPEC_RSP_INFO_FIELD_TYPE)); \
    EncodeZero(oriData, oriLen, encodedData +  sizeof(ftd_header) + sizeof(ftdc_crpheader), &encodedLen); \
    auto ftd_header_s = (ftd_header*) encodedData; \
    ftd_header_s->ftd_type = ftd_type_compressed; \
    ftd_header_s->ftd_ext_len = 0; \
    ftd_header_s->ftd_content_len = htons(encodedLen + sizeof(ftdc_crpheader)); \
    auto ftdc_crpheader_s = (ftdc_crpheader*)(encodedData + sizeof(ftd_header)); \
    ftdc_crpheader_s->protocol = ftdc_crp_protocol; \
    ftdc_crpheader_s->method = ftdc_cmp_compresszero; \
    auto senLen = encodedLen + sizeof(ftd_header) + sizeof(ftdc_crpheader); \
    TCP_SERVER_PTR->SendMsg(encodedData, senLen, SESSIONID, TOPICID);  \
}while(0) \



#define TCP_SEND_RTNINFO_WITHOUT_COMPRESS(TOPICID, TCP_SERVER_PTR, SESSIONID, FTDC_TID, SPEC_RSP_INFO_FIELD_TYPE, SPEC_RSP_INFO_FIELD_PTR,  SPEC_RSP_FID)     \
do {  \
    char oriData[2048] = {0};  \
    unsigned int oriLen = sizeof(ftdc_field_header) + sizeof(SPEC_RSP_INFO_FIELD_TYPE) + sizeof(ftdc_header);  \
    char encodedData[2048] = {0};  \
    unsigned int encodedLen = 0;  \
    char* data =  oriData + sizeof(ftd_header) + sizeof(ftdc_crpheader); \
    auto  ftdc_header_s = (ftdc_header *)data;    \
    ftdc_header_s->ftdc_version = ftdc_version_cur;  \
    ftdc_header_s->ftdc_chain = ftd_chain_last; \
    ftdc_header_s->ftdc_topic_id = htonl(FTDC_TID);  \
    ftdc_header_s->ftdc_seq_series = htons(ftdc_sid_none);  \
    ftdc_header_s->ftdc_seq_no = htonl(0);   \
    ftdc_header_s->ftdc_field_count = htons(1);   \
    ftdc_header_s->ftdc_content_len = htons(sizeof(ftdc_field_header) + sizeof(SPEC_RSP_INFO_FIELD_TYPE)); \
    ftdc_header_s->ftdc_req_id = htonl(0);  \
    data += sizeof(ftdc_header);   \
    auto header1 = (ftdc_field_header *)data; \
    header1->field_id = htons(SPEC_RSP_FID); \
    header1->field_len = htons(sizeof(SPEC_RSP_INFO_FIELD_TYPE)); \
    data += sizeof(ftdc_field_header); \
    auto specRspField = (SPEC_RSP_INFO_FIELD_TYPE *)data; \
    memcpy(specRspField, SPEC_RSP_INFO_FIELD_PTR, sizeof(SPEC_RSP_INFO_FIELD_TYPE)); \
    auto ftd_header_s = (ftd_header*) oriData; \
    ftd_header_s->ftd_type = ftd_type_none; \
    ftd_header_s->ftd_ext_len = 0; \
    ftd_header_s->ftd_content_len = htons(oriLen + sizeof(ftdc_crpheader)); \
    auto ftdc_crpheader_s = (ftdc_crpheader*)(oriData + sizeof(ftd_header)); \
    ftdc_crpheader_s->protocol = ftdc_crp_protocol; \
    ftdc_crpheader_s->method = ftdc_cmp_none; \
    auto senLen = oriLen + sizeof(ftd_header) + sizeof(ftdc_crpheader); \
    TCP_SERVER_PTR->SendMsg(oriData, senLen, SESSIONID, TOPICID);  \
}while(0) \


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
        int SendMsg(char * data, unsigned int len, SessionIdType sessionId, TOPICID_TYPE topicId);
};





#endif
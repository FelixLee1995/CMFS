/**
*  File: CTcpSession.h
*  Desc: Tcp
*  Created by felix on 2021/02/08
*/

#ifndef TCP_SESSION_H
#define TCP_SESSION_H
#pragma once

#include <iostream>
#include <set>
#include <deque>
#include <memory>
#include "asio.hpp"
#include "api/ctp_ext/ctp_ftdc_proto.h"
#include "api/ctp_ext/ctp_ext.h"
#include "tcp/CTcpServer.h"
#include "core/queue/concurrentqueue.h"
#include "ds/common.h"
#include "core/CFlow.h"

using asio::ip::tcp;
using namespace ctp_ftd;


class CTcpServer;

class CRevcBuffer
{
  private:
    unsigned char * m_Data;

  public:
    size_t m_Len;
    size_t m_LastLen;


  public:
    CRevcBuffer();

    ~CRevcBuffer();
    
    unsigned char * Data();

    size_t Length();
};


class MessageWrapper
{
public:
  enum { header_length = 4 };
  enum { max_body_length =  8192};
  uint8_t ext_len_;
  uint16_t body_length_;
  unsigned char data_[header_length + max_body_length];

  MessageWrapper();

  const unsigned char* data() const;

  unsigned char* data();

  std::size_t length() const;

  const unsigned char* body() const;

  unsigned char* body();
  std::size_t body_length() const;

  void body_length(std::size_t new_length);

  bool decode_header();

  void encode_header();

private:
 
};


typedef moodycamel::ConcurrentQueue<MessageWrapper> MessageQueue;

class CTcpCommunicator
{
public:
  virtual ~CTcpCommunicator() {}
  virtual void deliver(const MessageWrapper& msg) = 0;
};

typedef std::shared_ptr<CTcpCommunicator> communitor_ptr;



class CTcpSession : public CTcpCommunicator, public std::enable_shared_from_this<CTcpSession>
{
private:
    tcp::socket m_Socket;
    std::shared_ptr<CTcpServer> m_Server;
    CFlowManager::Sptr m_FlowManager;
    CRevcBuffer m_RecvBuffer;
    MessageWrapper m_Msg;
    MessageQueue m_MsgQueue;
    uint32_t m_SessionId;

public:
    using Sptr = std::shared_ptr<CTcpSession>;
    CTcpSession(tcp::socket socket, SessionIdType sessionId);

    void Start();
    void DoReadHeader();

    void do_read_body();

    void deliver(const MessageWrapper &msg);

    void DoWrite();
};

#endif
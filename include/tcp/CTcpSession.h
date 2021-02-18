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
  enum { max_body_length = 512 };
  uint16_t body_length_;
  uint8_t ext_len_;
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


typedef std::deque<MessageWrapper> MessageQueue;

class CTcpCommunicator
{
public:
  virtual ~CTcpCommunicator() {}
  virtual void deliver(const MessageWrapper& msg) = 0;
};

typedef std::shared_ptr<CTcpCommunicator> communitor_ptr;


class CTcpChatroom
{
public:
  void join(communitor_ptr participant);

  void leave(communitor_ptr participant);

private:
  std::set<communitor_ptr> m_Communicators;
  enum { max_recent_msgs = 128 };
  MessageQueue m_MsgQueue;
};


class CTcpSession : public CTcpCommunicator, public std::enable_shared_from_this<CTcpSession>
{
private:
    tcp::socket m_Socket;
    std::shared_ptr<CTcpChatroom> &m_ChatroomPtr;
    std::shared_ptr<CTcpServer> m_Server;
    CRevcBuffer m_RecvBuffer;
    MessageWrapper m_Msg;
    MessageQueue m_MsgQueue;
    uint32_t m_SessionId;

public:
    using Sptr = std::shared_ptr<CTcpSession>;
    CTcpSession(tcp::socket socket, std::shared_ptr<CTcpChatroom> chatroom, uint16_t sessionId);

    void Start();
    void DoReadHeader();

    void do_read_body();

    void deliver(const MessageWrapper &msg);

    void DoWrite();
};

#endif
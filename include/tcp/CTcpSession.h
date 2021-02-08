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

using asio::ip::tcp;
using namespace ctp_ftd;

class CRevcBuffer
{
  private:
    unsigned char * m_Data;

  public:
    size_t m_Len;
    size_t m_LastLen;


  public:
    CRevcBuffer()
    {
      m_Data = new unsigned char[1024];
    }

    ~CRevcBuffer()
    {
      delete []m_Data;
    }
    
    unsigned char * Data()
    {
      return m_Data;
    }

    size_t Length()
    {
      return m_Len;
    }
};


class MessageWrapper
{
public:
  enum { header_length = 4 };
  enum { max_body_length = 512 };
  uint16_t body_length_;
  uint8_t ext_len_;
  unsigned char data_[header_length + max_body_length];

  MessageWrapper()
    : body_length_(0)
  {
    memset(data_, 0, header_length + max_body_length);
  }

  const unsigned char* data() const
  {
    return data_;
  }

  unsigned char* data()
  {
    return data_;
  }

  std::size_t length() const
  {
    return header_length + body_length_;
  }

  const unsigned char* body() const
  {
    return data_ + header_length;
  }

  unsigned char* body()
  {
    return data_ + header_length;
  }

  std::size_t body_length() const
  {
    return body_length_;
  }

  void body_length(std::size_t new_length)
  {
    body_length_ = new_length;
    if (body_length_ > max_body_length)
      body_length_ = max_body_length;
  }

  bool decode_header()
  {
    
    memcpy(&body_length_, data_ + 3, 1);
    memcpy(&body_length_+1, data_ + 2, 1);

    body_length_ -= 1;
    if (body_length_ > max_body_length)
    {
      body_length_ = 5;
    }
    return true;
  }

  void encode_header()
  {
    unsigned char header[header_length + 1] = "";
    //std::sprintf(header, "%4d", static_cast<int>(body_length_));
    std::memcpy(data_, header, header_length);
  }

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
  void join(communitor_ptr participant)
  {
    m_Communicators.insert(participant);
    for (auto msg: m_MsgQueue)
      participant->deliver(msg);
  }

  void leave(communitor_ptr participant)
  {
    m_Communicators.erase(participant);
  }


private:
  std::set<communitor_ptr> m_Communicators;
  enum { max_recent_msgs = 128 };
  MessageQueue m_MsgQueue;
};

class CTcpSession : public CTcpCommunicator, public std::enable_shared_from_this<CTcpSession>
{
private:
    tcp::socket m_Socket;
    CTcpChatroom &m_Chatroom;
    CRevcBuffer m_RecvBuffer;
    MessageWrapper m_Msg;
    MessageQueue m_MsgQueue;

public:
    CTcpSession(tcp::socket socket, CTcpChatroom &chatroom)
        : m_Socket(std::move(socket)), m_Chatroom(chatroom), m_RecvBuffer()
    {
    }

    void Start()
    {
        m_Chatroom.join(shared_from_this());
        DoReadHeader();
    }

    void DoReadHeader()
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

    void do_read_body()
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

                    std::cout << "topicid is " << topicID << " seriesID is " << seriesID << " seq_no is " << seq_no
                              << " req_id is " << req_id << std::endl;

                    char *content = (char *)ftdcheader + sizeof(ftdc_header);

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
                            MessageWrapper msg;
                            memcpy(msg.data_, bLoginRsp, sizeof(bLoginRsp));
                            msg.body_length_ = sizeof(bLoginRsp) - 4;
                            deliver(msg);
                            break;
                        }
                        case ftdc_fid_ReqSub:
                        {
                            MessageWrapper msg;
                            memcpy(msg.data_, bRspSub, sizeof(bRspSub));
                            msg.body_length_ = sizeof(bRspSub) - 4;
                            deliver(msg);
                            // MessageWrapper msg1;
                            // memcpy(msg1.data_, bRtnMarket, sizeof(bRtnMarket));
                            // msg1.body_length_ = sizeof(bRtnMarket) - 4;
                            // deliver(msg1);

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
                    m_Chatroom.leave(shared_from_this());
                }
            });
    }

    void deliver(const MessageWrapper &msg)
    {
        bool write_in_progress = !m_MsgQueue.empty();
        m_MsgQueue.push_back(msg);
        if (!write_in_progress)
        {
            std::cout << "before do write , threadid is " << std::this_thread::get_id() << std::endl;

            DoWrite();
        }
    }

    void DoWrite()
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
                    m_Chatroom.leave(shared_from_this());
                }
            });
    }
};

#endif
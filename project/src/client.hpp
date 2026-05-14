#pragma once

#include <zf_common_headfile.hpp>

#include "utils.hpp"

class Client {
public:
  virtual void send(const uint8 *data, size_t size) = 0;
  // try to recv up to `size` bytes. Returns number of bytes read, 0 for no data, -1 for error.
  virtual int recv(uint8 *buffer, size_t size) = 0;

  virtual ~Client() = default;
};

class TcpClient : public Client {
public:
  TcpClient() = delete;
  TcpClient(const char *addr, uint16_t port) : addr_(addr), port_(port) {}

  int8 init();

  void send(const uint8 *data, size_t size) override;
  int recv(uint8 *buffer, size_t size) override;

private:
  const char *addr_;
  uint16_t port_;

  std::unique_ptr<zf_driver_tcp_client> tcp_client_{std::make_unique<zf_driver_tcp_client>()};
};

int8 TcpClient::init()
{
  return tcp_client_->init(addr_, port_);
}

void TcpClient::send(const uint8 *data, size_t size)
{
  tcp_client_->send_data(data, size);
}

int TcpClient::recv(uint8 *buffer, size_t size)
{
  return (int) tcp_client_->read_data(buffer, (uint32) size);
}

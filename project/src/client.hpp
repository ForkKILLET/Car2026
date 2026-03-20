#include <zf_common_headfile.hpp>
#include "utils.hpp"

class fp_client {
public:
  virtual void send(const uint8* data, size_t size) = 0;
  virtual void recv(uint8* buffer, size_t size) = 0;

  virtual ~fp_client() = default;
};

class fp_tcp_client : public fp_client {
public:
  fp_tcp_client(const char* addr, uint16_t port) :
    addr_(addr), port_(port)
  {}

  int8 init();

  void send(const uint8* data, size_t size) override;
  void recv(uint8* buffer, size_t size) override;

private:
  const char* addr_;
  uint16_t port_;

  zf_driver_tcp_client tcp_client_;
};

int8 fp_tcp_client::init() {
  return tcp_client_.init(addr_, port_);
}

void fp_tcp_client::send(const uint8* data, size_t size) {
  tcp_client_.send_data(data, size);
}

void fp_tcp_client::recv(uint8* buffer, size_t size) {
  tcp_client_.read_data(buffer, size);
}

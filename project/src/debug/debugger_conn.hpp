#pragma once

#include <zf_common_headfile.hpp>

#include "client.hpp"
#include "utils.hpp"

TcpClient conn_debugger()
{
  auto addr = getenv("DEBUGGER_ADDR");
  auto port_str = getenv("DEBUGGER_PORT");

  if (! addr || ! port_str) {
    fp_err("Missing DEBUGGER_ADDR and/or DEBUGGER_PORT\n");
    exit(1);
  }

  auto port = static_cast<uint16_t>(std::stoi(port_str));
  if (port == 0) {
    fp_err("Invalid SERVER_PORT: %s\n", port_str);
    exit(1);
  }

  TcpClient client{addr, port};
  if (client.init() < 0) {
    fp_err("Failed to initialize TCP client\n");
    exit(1);
  }
  fp_log("TCP client initialized\n");

  return client;
}

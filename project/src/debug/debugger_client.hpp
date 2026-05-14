#pragma once

#include <zf_common_headfile.hpp>

#include "client.hpp"
#include "utils.hpp"

TcpClient &get_debugger_client(uint16 port)
{
  auto addr = getenv("DEBUGGER_ADDR");

  if (! addr) {
    fp_err("Missing DEBUGGER_ADDR\n");
    exit(1);
  }

  static TcpClient client{addr, port};
  fp_log("Connecting to debugger server at %s:%u...\n", addr, port);
  if (client.init() < 0) {
    fp_err("Failed to initialize TCP client\n");
    exit(1);
  }
  fp_log("Connected to debugger server.\n");

  return client;
}

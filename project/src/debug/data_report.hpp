#pragma once

#include <cstring>
#include <initializer_list>
#include <zf_common_headfile.hpp>

#include "client.hpp"
#include "debug/debugger_client.hpp"
#include "utils.hpp"

class DebugDataReport {
public:
  DebugDataReport() = delete;

  static DebugDataReport &instance()
  {
    static DebugDataReport report{get_debugger_client(2028)};
    return report;
  }

  void send_data(std::initializer_list<float> values)
  {
    const size_t count = values.size();
    if (count == 0) {
      return;
    }
    if (count > kMaxValues) {
      fp_err("DebugDataReport: too many values (%u > %u)\n",
             static_cast<unsigned>(count),
             static_cast<unsigned>(kMaxValues));
      return;
    }

    uint8 buffer[kHeaderSize + kMaxValues * sizeof(float)];
    buffer[0] = kMagic0;
    buffer[1] = kMagic1;
    buffer[2] = static_cast<uint8>(count);

    size_t offset = kHeaderSize;
    for (float value : values) {
      std::memcpy(buffer + offset, &value, sizeof(float));
      offset += sizeof(float);
    }

    client_.send(buffer, offset);
  }

private:
  DebugDataReport(Client &client) : client_(client) {}

  // Frame format: [0xDA 0x7A][count][count * float32 (little-endian)]
  static constexpr uint8 kMagic0 = 0xDA;
  static constexpr uint8 kMagic1 = 0x7A;
  static constexpr size_t kHeaderSize = 3;
  static constexpr size_t kMaxValues = 32;

  Client &client_;
};

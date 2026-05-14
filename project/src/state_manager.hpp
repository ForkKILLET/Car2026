#pragma once

#include "zf_common_typedef.hpp"

#include "control_manager.hpp"

// 状态
typedef enum ControlState {
  STATE_STOP,      // 停车
  STATE_READY,     // 准备运行
  STATE_RUN,       // 正常运行
  STATE_LOST_LINE, // 丢线
  STATE_ERROR      // 异常
};

class StateManager {
public:
  // 初始化
  StateManager()
  {
    auto &p = g_params.state_machine;

    state = STATE_STOP;
    last_state = STATE_STOP;

    start_cmd = 0;
    error_flag = 0;

    lost_line_count = 0;
    lost_line_limit = p.lost_line_limit;
  }

  // 当前状态
  ControlState state = STATE_STOP;

  // 上一个状态
  ControlState last_state = STATE_STOP;

  // 启动命令
  uint8 start_cmd = 0;

  // 异常标志
  uint8 error_flag = 0;

  // 丢线计数
  uint32 lost_line_count = 0;

  // 丢线允许持续的最大周期数
  uint32 lost_line_limit = 10;
};

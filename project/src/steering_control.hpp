#pragma once
#include "control_params.hpp"
#include "zf_common_typedef.hpp"
#include <algorithm>
#include <cmath>

class SteeringControl {
public:
  // 初始化
  void init(const SteeringParams &params)
  {
    kp = params.kp;
    kd = params.kd;
    turn_limit = params.turn_limit;
    deadband = params.deadband;
  }

  // 清零
  void clear()
  {
    error = 0.0f;
    error_last = 0.0f;
    turn = 0.0f;
  }

  // 更新方向控制
  void update(float target_error, float current_error)
  {
    float delta_error;

    error = target_error - current_error;

    // 小误差死区
    if (std::fabs(error) < deadband)
      error = 0.0f;

    delta_error = error - error_last;

    // PD 输出
    turn = kp * error + kd * delta_error;

    // 输出限幅
    turn = std::clamp(turn, -turn_limit, turn_limit);

    error_last = error;
  }

  // 输入当前偏差
  void run(float line_error)
  {
    update(0.0f, line_error);
  }

  float get_turn() const
  {
    return turn;
  }

private:
  // pd 参数
  float kp = 0.0f;
  float kd = 0.0f;

  // 当前误差、上一次误差
  float error = 0.0f;
  float error_last = 0.0f;

  // 输出 turn
  float turn = 0.0f;

  // 输出限幅
  float turn_limit = 1.0f;

  // 小误差死区
  float deadband = 0.0f;
};

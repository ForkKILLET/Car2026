#pragma once
#include <algorithm>
#include <cmath>

#include "zf_common_typedef.hpp"

#include "control_params.hpp"

class SteeringControl {
public:
  // 初始化
  SteeringControl()
  {
    auto &p = g_params.steering;

    kp = p.kp;
    kd = p.kd;
    turn_ratio = p.turn_ratio;
    turn_limit = p.turn_limit;
    deadband = p.deadband;

    clear();
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
    turn = turn_ratio * (kp * error + kd * delta_error);

    // 输出限幅
    turn = limit(turn, turn_limit);

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
  // PD 参数
  float kp;
  float kd;

  // 当前误差、上一次误差
  float error;
  float error_last;

  // 转向输出
  float turn;

  // 循线误差:转向输出 比例
  float turn_ratio;

  // 输出限幅
  float turn_limit;

  // 小误差死区
  float deadband;
};

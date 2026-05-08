#pragma once

#include "control_params.hpp"
#include "pid.hpp"
#include "zf_common_typedef.hpp"

class YawRateControl {
public:
  // 初始化
  void init(const YawRatePidParams &params)
  {
    pid.set(PID_POS, params.kp, params.ki, params.kd);
    pid.set_limit(params.error_limit, params.turn_limit);

    yaw_rate_target = 0.0f;
    yaw_rate_current = 0.0f;

    turn = 0.0f;
    turn_limit = params.turn_limit;
  }

  // 清零
  void clear()
  {
    yaw_rate_target = 0.0f;
    yaw_rate_current = 0.0f;
    turn = 0.0f;

    pid.clean();
  }

  // 更新
  void update(float yaw_rate_target, float yaw_rate_current)
  {
    this->yaw_rate_target = yaw_rate_target;
    this->yaw_rate_current = yaw_rate_current;

    pid.position(this->yaw_rate_target, this->yaw_rate_current);

    turn = pid.get_output_pos();

    if (turn > turn_limit) {
      turn = turn_limit;
    }
    else if (turn < -turn_limit) {
      turn = -turn_limit;
    }
  }

  // 一步运行
  void run(float yaw_rate_target, float yaw_rate_current)
  {
    update(yaw_rate_target, yaw_rate_current);
  }

  float get_turn() const
  {
    return turn;
  }

private:
  PidCtrl pid{};

  float yaw_rate_target = 0.0f;
  float yaw_rate_current = 0.0f;

  float turn = 0.0f;
  float turn_limit = 1.0f;
};

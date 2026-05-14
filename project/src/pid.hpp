#pragma once
#include <cmath>

#include "zf_common_typedef.hpp"

enum class PidType {
  Pos, // 位置式PID
  Inc, // 增量式PID
};

class PidCtrl {
public:
  void set(PidType type, float kp, float ki, float kd, float output_ratio = 1.0f)
  {
    this->type = type;

    this->kp = kp;
    this->ki = ki;
    this->kd = kd;
    this->output_ratio = output_ratio;

    error_previous = 0.0f;
    error_last = 0.0f;
    error = 0.0f;

    error_sum = 0.0f;
    error_limit = 1000.0f;
    error_zero_count = 0;

    output_inc = 0.0f;
    output_pos = 0.0f;
    output_limit = 1000.0f;
  }

  void set_limit(float error_limit, float output_limit)
  {
    this->error_limit = error_limit;
    this->output_limit = output_limit;
  }

  void position(float target, float current)
  {
    float delta_error;

    error = target - current;

    float deadband = 0.5f; // 死区处理，阈值可调
    if (std::fabs(error) < deadband) {
      error = 0.0f;
    }

    error_sum += error;

    // 若误差长时间为0，则清空积分项
    if (error == 0.0f) {
      error_zero_count++;
      if (error_zero_count > 50) {
        error_sum = 0.0f;
        error_zero_count = 0;
      }
    }
    else {
      error_zero_count = 0;
    }

    // 积分限幅
    if (error_sum > error_limit) {
      error_sum = error_limit;
    }
    else if (error_sum < -error_limit) {
      error_sum = -error_limit;
    }

    // 微分项
    delta_error = error - error_last;

    output_pos = output_ratio * (kp * error + ki * error_sum + kd * delta_error);

    // 输出限幅
    output_pos = limit(output_pos, output_limit);

    error_last = error;
  }

  void increment(float target, float current)
  {
    float delta_output;

    error = target - current;

    delta_output = output_ratio * (kp * (error - error_last) + ki * error +
                                   kd * (error - 2.0f * error_last + error_previous));

    output_inc += delta_output;

    // 输出限幅
    output_inc = limit(output_inc, output_limit);

    error_previous = error_last;
    error_last = error;
  }

  void clear()
  {
    error_previous = 0.0f;
    error_last = 0.0f;
    error = 0.0f;
    error_sum = 0.0f;
    error_zero_count = 0;
    output_inc = 0.0f;
    output_pos = 0.0f;
  }

  float get_output_pos() const
  {
    return output_pos;
  }

  float get_output_inc() const
  {
    return output_inc;
  }

private:
  PidType type = PidType::Pos; // PID类型

  float kp;
  float ki;
  float kd;
  float output_ratio; // 输出缩放比例
  float output_limit; // 输出限幅

  float error_previous; // 上上次误差
  float error_last;     // 上次误差
  float error;          // 当前误差

  float error_sum;
  float error_limit;       // 积分限幅
  uint32 error_zero_count; // 误差连续为0的计数

  float output_pos; // 位置式pid输出
  float output_inc; // 增量
};

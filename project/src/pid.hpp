#pragma once
#include <cmath>
#include "zf_common_typedef.hpp"

enum pid_type {
  PID_POS, // 位置式PID
  PID_INC, // 增量式PID
};

class PidCtrl {
public:
  void set(pid_type type, float kp, float ki, float kd)
  {
    this->type = type;

    this->kp = kp;
    this->ki = ki;
    this->kd = kd;

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

    output_pos = kp * error + ki * error_sum + kd * delta_error;

    // 输出限幅
    if (output_pos > output_limit) {
      output_pos = output_limit;
    }
    else if (output_pos < -output_limit) {
      output_pos = -output_limit;
    }

    error_last = error;
  }

  void increment(float target, float current)
  {
    float delta_output;

    error = target - current;

    delta_output =
        kp * (error - error_last) + ki * error + kd * (error - 2.0f * error_last + error_previous);

    output_inc += delta_output;

    // 输出限幅
    if (output_inc > output_limit) {
      output_inc = output_limit;
    }
    else if (output_inc < -output_limit) {
      output_inc = -output_limit;
    }

    error_previous = error_last;
    error_last = error;
  }

  void clean()
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
  pid_type type = PID_POS; // PID类型

  float kp = 0.0f;
  float ki = 0.0f;
  float kd = 0.0f;

  float error_previous = 0.0f; // 上上次误差
  float error_last = 0.0f;     // 上次误差
  float error = 0.0f;          // 当前误差

  float error_sum = 0.0f;
  float error_limit = 1000.0f; // 积分限幅
  uint32 error_zero_count = 0; // 误差连续为0的计数

  float output_pos = 0.0f;      // 位置式pid输出
  float output_inc = 0.0f;      // 增量
  float output_limit = 1000.0f; // 输出限幅
};

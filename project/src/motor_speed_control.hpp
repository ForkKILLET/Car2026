#pragma once
#include "zf_common_typedef.hpp"

#include "control_params.hpp"
#include "motor.hpp"
#include "pid.hpp"
#include "utils.hpp"

// 单个电机速度闭环控制
// 目标速度 - 当前速度 -> 增量 pid -> pwm 输出

class MotorSpeedCtrl {
public:
  // 初始化
  void init(const MotorSpeedPidParams &params)
  {
    pid.set(PID_INC, params.kp, params.ki, params.kd);
    pid.set_limit(params.error_limit, static_cast<float>(params.pwm_limit));

    target_speed = 0.0f;
    current_speed = 0.0f;
    pwm_out = 0;
    pwm_limit = params.pwm_limit;
    pwm_deadzone = params.pwm_deadzone;
  }

  // 设置目标速度
  void set_target(float target_speed)
  {
    this->target_speed = target_speed;
  }

  // 设置当前速度
  void set_current(float current_speed)
  {
    this->current_speed = current_speed;
  }

  // 执行一次闭环更新
  void update(Motor &motor)
  {
    // 增量 pid
    pid.increment(target_speed, current_speed);
    pwm_out = static_cast<int16>(pid.get_output_inc());

    // 限幅
    pwm_out = limit(pwm_out, pwm_limit);

    // 死区
    if (std::abs(pwm_out) < pwm_deadzone)
      pwm_out = 0;

    // 输出到电机
    motor.set_duty(pwm_out);
  }

  // 停止闭环控制
  void stop(Motor &motor)
  {
    target_speed = 0.0f;
    current_speed = 0.0f;
    pwm_out = 0;

    pid.clear();
    motor.set_duty(0);
  }

  int16 get_pwm_out() const
  {
    return pwm_out;
  }

private:
  // pid 控制器
  PidCtrl pid{};

  // 目标速度（m/s）
  float target_speed = 0.0f;

  // 当前速度（m/s）
  float current_speed = 0.0f;

  // 输出 pwm
  int16 pwm_out = 0;

  // pwm 限幅
  int16 pwm_limit = 1000;

  // pwm 死区
  int16 pwm_deadzone = 700;
};

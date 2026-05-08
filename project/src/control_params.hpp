#pragma once

#include "zf_common_typedef.hpp"

struct EncoderParams {
  float meter_per_count_l = 0.201f / 1907.0f;
  float meter_per_count_r = -0.201f / 1907.0f; // 右轮编码器反向安装
  float wheel_base = 0.20f;
  float alpha = 0.50f;
  int timer_ms = 20;
};

struct MotorSpeedPidParams {
  float kp = 50.0f;
  float ki = 5.0f;
  float kd = 1.0f;
  float error_limit = 1000.0f;
  int16 pwm_limit = 5000;
  int16 pwm_deadzone = 700;
};

struct SteeringParams {
  float kp = 0.8f;
  float kd = 0.2f;
  float turn_limit = 0.60f;
  float deadband = 0.0f;
};

struct YawRatePidParams {
  float kp = 1.2f;
  float ki = 0.0f;
  float kd = 0.08f;
  float error_limit = 1000.0f;
  float turn_limit = 0.15f;
};

struct DiffDriveParams {};

struct SpeedPlanParams {
  float base_speed_target = 0.30f;
  float base_speed_min = 0.15f;
  float turn_slow_gain = 1.0f;
  float speed_ramp = 0.50f;
};

struct GyroParams {
  float gyro_scale = 16.4f;
  int bias_sample_count = 200;
  int bias_sample_delay_ms = 2;
};

struct StateMachineParams {
  uint32 lost_line_limit = 10;
};

struct ControlParams {
  EncoderParams encoder;
  MotorSpeedPidParams motor_l_pid;
  MotorSpeedPidParams motor_r_pid;
  SteeringParams steering;
  YawRatePidParams yaw_rate;
  DiffDriveParams diff_drive;
  SpeedPlanParams speed_plan;
  GyroParams gyro;
  StateMachineParams state_machine;
};

static ControlParams params{};

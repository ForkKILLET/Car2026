#pragma once

#include <INIReader.hpp>
#include <string>
#include <zf_common_typedef.hpp>

#include "utils.hpp"

struct EncoderParams {
  float meter_per_count_l = -0.201f / 1907.0f; // 左轮编码器反向安装
  float meter_per_count_r = +0.201f / 1907.0f;
  float wheel_base = 0.20f;
  float alpha = 0.50f;
};

struct MotorSpeedPidParams {
  float kp = 1.0f;
  float ki = 5.0f;
  float kd = 0.1f;
  float output_ratio = 7.5f;
  float error_limit = 1000.0f;
  int16 pwm_limit = 5000;
  int16 pwm_deadzone = 700;
};

struct SteeringParams {
  float kp = 0.8f;
  float kd = 0.1f;
  float turn_ratio = 0.2f;
  float turn_limit = 6.0f;
  float deadband = 0.0f;
  uint8 period_ratio = 10;
};

struct YawRatePidParams {
  float kp = 1.2f;
  float ki = 0.0f;
  float kd = 0.08f;
  float error_limit = 1000.0f;
  float turn_limit = 0.15f;
  uint8 period_ratio = 5;
};

struct DiffDriveParams {};

struct SpeedPlanParams {
  float base_speed_target = 0.3f;
  float base_speed_min = 0.0f;
  float turn_slow_gain = 0.0f;
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

struct DebugParams {
  bool enable_data_report = false;
};

struct TimerParams {
  uint8 encoder_period_ms = 2;
  uint8 control_period_ms = 2;
};

struct CtrlParams {
  EncoderParams encoder;
  MotorSpeedPidParams motor_l_pid;
  MotorSpeedPidParams motor_r_pid;
  SteeringParams steering;
  YawRatePidParams yaw_rate;
  DiffDriveParams diff_drive;
  SpeedPlanParams speed_plan;
  GyroParams gyro;
  StateMachineParams state_machine;
  TimerParams timer;
  DebugParams debug;
};

static CtrlParams load_params(const char *filename)
{
  CtrlParams params;

  INIReader reader(filename);
  int error = reader.ParseError();
  if (error == -1) {
    fp_err("Failed to open params file: %s\n", filename);
    return params;
  }
  else if (error > 0) {
    fp_err("Failed to parse params file: %s (error at line %d)\n", filename, error);
    return params;
  }

  fp_log("Successfully loaded params file: %s\n", filename);

  // encoder
  reader.EmplaceFloat("encoder", "meter_per_count_l", params.encoder.meter_per_count_l);
  reader.EmplaceFloat("encoder", "meter_per_count_r", params.encoder.meter_per_count_r);
  reader.EmplaceFloat("encoder", "wheel_base", params.encoder.wheel_base);
  reader.EmplaceFloat("encoder", "alpha", params.encoder.alpha);

  // motor pid
  reader.EmplaceFloat("motor_pid", "kp", params.motor_l_pid.kp);
  reader.EmplaceFloat("motor_pid", "ki", params.motor_l_pid.ki);
  reader.EmplaceFloat("motor_pid", "kd", params.motor_l_pid.kd);
  reader.EmplaceFloat("motor_pid", "output_ratio", params.motor_l_pid.output_ratio);
  reader.EmplaceFloat("motor_pid", "error_limit", params.motor_l_pid.error_limit);
  reader.EmplaceInteger("motor_pid", "pwm_limit", params.motor_l_pid.pwm_limit);
  reader.EmplaceInteger("motor_pid", "pwm_deadzone", params.motor_l_pid.pwm_deadzone);

  reader.EmplaceFloat("motor_pid", "kp", params.motor_r_pid.kp);
  reader.EmplaceFloat("motor_pid", "ki", params.motor_r_pid.ki);
  reader.EmplaceFloat("motor_pid", "kd", params.motor_r_pid.kd);
  reader.EmplaceFloat("motor_pid", "output_ratio", params.motor_r_pid.output_ratio);
  reader.EmplaceFloat("motor_pid", "error_limit", params.motor_r_pid.error_limit);
  reader.EmplaceInteger("motor_pid", "pwm_limit", params.motor_r_pid.pwm_limit);
  reader.EmplaceInteger("motor_pid", "pwm_deadzone", params.motor_r_pid.pwm_deadzone);

  // steering
  reader.EmplaceFloat("steering", "kp", params.steering.kp);
  reader.EmplaceFloat("steering", "kd", params.steering.kd);
  reader.EmplaceFloat("steering", "turn_ratio", params.steering.turn_ratio);
  reader.EmplaceFloat("steering", "turn_limit", params.steering.turn_limit);
  reader.EmplaceFloat("steering", "deadband", params.steering.deadband);
  reader.EmplaceInteger("steering", "period_ratio", params.steering.period_ratio);

  // yaw rate
  reader.EmplaceFloat("yaw_rate", "kp", params.yaw_rate.kp);
  reader.EmplaceFloat("yaw_rate", "ki", params.yaw_rate.ki);
  reader.EmplaceFloat("yaw_rate", "kd", params.yaw_rate.kd);
  reader.EmplaceFloat("yaw_rate", "error_limit", params.yaw_rate.error_limit);
  reader.EmplaceFloat("yaw_rate", "turn_limit", params.yaw_rate.turn_limit);
  reader.EmplaceInteger("yaw_rate", "period_ratio", params.yaw_rate.period_ratio);

  // speed_plan
  reader.EmplaceFloat("speed_plan", "base_speed_target", params.speed_plan.base_speed_target);
  reader.EmplaceFloat("speed_plan", "base_speed_min", params.speed_plan.base_speed_min);
  reader.EmplaceFloat("speed_plan", "turn_slow_gain", params.speed_plan.turn_slow_gain);
  reader.EmplaceFloat("speed_plan", "speed_ramp", params.speed_plan.speed_ramp);

  // gyro
  reader.EmplaceFloat("gyro", "gyro_scale", params.gyro.gyro_scale);
  reader.EmplaceInteger("gyro", "bias_sample_count", params.gyro.bias_sample_count);
  reader.EmplaceInteger("gyro", "bias_sample_delay_ms", params.gyro.bias_sample_delay_ms);

  // state_machine
  reader.EmplaceInteger("state_machine", "lost_line_limit", params.state_machine.lost_line_limit);

  // timer
  reader.EmplaceInteger("timer", "encoder_period_ms", params.timer.control_period_ms);
  reader.EmplaceInteger("timer", "control_period_ms", params.timer.control_period_ms);

  // debug
  reader.EmplaceBoolean("debug", "enable_data_report", params.debug.enable_data_report);

  return params;
}

static CtrlParams g_params = load_params("/home/root/params.ini");

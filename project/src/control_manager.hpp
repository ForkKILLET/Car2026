#pragma once

#include "zf_common_typedef.hpp"
#include "steering_control.hpp"
#include "diff_drive.hpp"
#include "chassis_control.hpp"
#include "gyro_interface.hpp"
#include "yaw_rate_control.hpp"
#include <cmath>
#include "control_params.hpp"

struct control_manager
{
  // 外环：中线偏差 -> 目标角速度
  steering_control steer_ctrl;

  // 内环：目标角速度 -> turn
  yaw_rate_control yaw_ctrl;

  // 差速解算
  diff_drive drive;

  // 陀螺仪接口
  gyro_interface gyro;

  // 图像输入
  float line_error;
  bool line_valid;

  // 速度规划
  float base_speed;
  float base_speed_target;
  float base_speed_min;
  float turn_slow_gain;
  float speed_ramp;

  // 外环输出
  float yaw_rate_target;

  // 内环输出
  float turn;

  // 启停
  bool enable;

  control_manager()
  {
    line_error = 0.0f;
    line_valid = 0;

    base_speed = 0.0f;
    base_speed_target = 0.30f;
    base_speed_min = 0.15f;
    turn_slow_gain = 1.0f;
    speed_ramp = 0.50f;

    yaw_rate_target = 0.0f;
    turn = 0.0f;

    enable = false;
  }
};

// 初始化
static inline void control_manager_init(control_manager &ctrl)
{
  // 底盘初始化
  chassis_control_init();

  // 外环初始化：中线偏差 -> 目标角速度
  steering_control_init(ctrl.steer_ctrl,
                        params.steering.kp,
                        params.steering.kd,
                        params.steering.output_limit,
                        params.steering.deadband);

  // 内环初始化：目标角速度 -> turn
  yaw_rate_control_init(ctrl.yaw_ctrl,
                        params.yaw_rate.kp,
                        params.yaw_rate.ki,
                        params.yaw_rate.kd,
                        params.yaw_rate.error_limit,
                        params.yaw_rate.turn_limit);

  // 差速解算初始化
  diff_drive_init(ctrl.drive);

  // 陀螺仪初始化
  gyro_interface_init(ctrl.gyro);

  ctrl.line_error = 0.0f;
  ctrl.line_valid = 0;

  ctrl.base_speed_target = params.speed_plan.base_speed_target;
  ctrl.base_speed_min = params.speed_plan.base_speed_min;
  ctrl.turn_slow_gain = params.speed_plan.turn_slow_gain;
  ctrl.speed_ramp = params.speed_plan.speed_ramp;

  ctrl.yaw_rate_target = 0.0f;
  ctrl.turn = 0.0f;

  ctrl.enable = false;

  chassis_stop();
}

// 设置图像输入
static inline void control_manager_set_line(control_manager &ctrl,
                                            float line_error,
                                            uint8 line_valid)
{
  ctrl.line_error = line_error;
  ctrl.line_valid = line_valid;
}

// 更新陀螺仪
static inline void control_manager_update_gyro(control_manager &ctrl)
{
  gyro_interface_update(ctrl.gyro);
}

// 设置速度规划参数
static inline void control_manager_set_speed_plan(control_manager &ctrl,
                                                  float base_speed_target,
                                                  float base_speed_min,
                                                  float turn_slow_gain,
                                                  float speed_ramp)
{
  ctrl.base_speed_target = base_speed_target;
  ctrl.base_speed_min = base_speed_min;
  ctrl.turn_slow_gain = turn_slow_gain;
  ctrl.speed_ramp = speed_ramp;
}

// 启动
static inline void control_manager_start(control_manager &ctrl)
{
  ctrl.enable = true;
}

// 停止
static inline void control_manager_stop(control_manager &ctrl)
{
  ctrl.enable = false;
  ctrl.yaw_rate_target = 0.0f;
  ctrl.turn = 0.0f;
  ctrl.base_speed = 0.0f;

  steering_control_clear(ctrl.steer_ctrl);
  yaw_rate_control_clear(ctrl.yaw_ctrl);
  chassis_stop();
}

// 清零
static inline void control_manager_clear(control_manager &ctrl)
{
  ctrl.line_error = 0.0f;
  ctrl.line_valid = 0;

  ctrl.base_speed = 0.0f;
  ctrl.yaw_rate_target = 0.0f;
  ctrl.turn = 0.0f;
  ctrl.enable = 0;

  steering_control_clear(ctrl.steer_ctrl);
  yaw_rate_control_clear(ctrl.yaw_ctrl);
  gyro_interface_clear(ctrl.gyro);
  chassis_clear();
}

// 获取目标角速度
static inline float control_manager_get_yaw_rate_target(const control_manager &ctrl)
{
  return ctrl.yaw_rate_target;
}

// 获取实际角速度
static inline float control_manager_get_yaw_rate_current(const control_manager &ctrl)
{
  return gyro_interface_get_yaw_rate(ctrl.gyro);
}

// 获取当前 turn
static inline float control_manager_get_turn(const control_manager &ctrl)
{
  return ctrl.turn;
}

// 获取当前基础速度
static inline float control_manager_get_base_speed(const control_manager &ctrl)
{
  return ctrl.base_speed;
}

// 核心控制循环
static inline void control_manager_loop(control_manager &ctrl, float dt_s)
{
  float base_speed_plan;
  float turn_for_speed_plan;

  // 没启动就停车
  if (!ctrl.enable)
  {
    chassis_stop();
    return;
  }

  // 丢线就停车
  if (!ctrl.line_valid)
  {
    ctrl.yaw_rate_target = 0.0f;
    ctrl.turn = 0.0f;
    ctrl.base_speed = 0.0f;

    steering_control_clear(ctrl.steer_ctrl);
    yaw_rate_control_clear(ctrl.yaw_ctrl);
    chassis_stop();
    return;
  }

  // 更新陀螺仪
  control_manager_update_gyro(ctrl);

  // 外环：中线偏差 -> 目标角速度
  steering_control_run(ctrl.steer_ctrl, ctrl.line_error);
  ctrl.yaw_rate_target = ctrl.steer_ctrl.turn;

  // 内环：目标角速度 + 当前角速度 -> turn
  if (ctrl.gyro.valid != 0)
  {
    yaw_rate_control_run(ctrl.yaw_ctrl,
                         ctrl.yaw_rate_target,
                         gyro_interface_get_yaw_rate(ctrl.gyro));

    ctrl.turn = ctrl.yaw_ctrl.turn;
    turn_for_speed_plan = std::fabs(ctrl.yaw_rate_target);
  }
  else
  {
    ctrl.turn = ctrl.yaw_rate_target;
    turn_for_speed_plan = std::fabs(ctrl.turn);
  }

  // 速度规划：转弯时降速
  base_speed_plan = ctrl.base_speed_target - ctrl.turn_slow_gain * turn_for_speed_plan;

  if (base_speed_plan < ctrl.base_speed_min)
  {
    base_speed_plan = ctrl.base_speed_min;
  }

  if (base_speed_plan > ctrl.base_speed_target)
  {
    base_speed_plan = ctrl.base_speed_target;
  }

  // 基础速度斜坡
  // if (dt_s > 0.0f)
  // {
  //   max_step = ctrl.speed_ramp * dt_s;

  //   if (ctrl.base_speed < base_speed_plan)
  //   {
  //     ctrl.base_speed += max_step;
  //     if (ctrl.base_speed > base_speed_plan)
  //     {
  //       ctrl.base_speed = base_speed_plan;
  //     }
  //   }
  //   else if (ctrl.base_speed > base_speed_plan)
  //   {
  //     ctrl.base_speed -= max_step;
  //     if (ctrl.base_speed < base_speed_plan)
  //     {
  //       ctrl.base_speed = base_speed_plan;
  //     }
  //   }
  // }

  ctrl.base_speed = base_speed_plan;

  // 差速解算
  diff_drive_loop(ctrl.drive, ctrl.base_speed, ctrl.turn);

  // 左右轮闭环
  chassis_control_loop();
}

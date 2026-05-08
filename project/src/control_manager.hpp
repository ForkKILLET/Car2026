#pragma once

#include <algorithm>
#include <cmath>

#include "zf_common_typedef.hpp"

#include "chassis_control.hpp"
#include "control_params.hpp"
#include "diff_drive.hpp"
#include "gyro_interface.hpp"
#include "steering_control.hpp"
#include "yaw_rate_control.hpp"

class ControlManager {
public:
  ControlManager() = default;

  // 初始化
  void init(const ControlParams &params)
  {
    // 底盘初始化
    chassis.init(params);

    // 外环初始化：中线偏差 -> 目标角速度
    steer_ctrl.init(params.steering);

    // 内环初始化：目标角速度 -> turn
    yaw_ctrl.init(params.yaw_rate);

    // 差速解算初始化
    drive.init(params.diff_drive);

    // 陀螺仪初始化
    gyro.init(params.gyro);

    line_error = 0.0f;
    line_valid = false;

    base_speed_target = params.speed_plan.base_speed_target;
    base_speed_min = params.speed_plan.base_speed_min;
    turn_slow_gain = params.speed_plan.turn_slow_gain;
    speed_ramp = params.speed_plan.speed_ramp;

    yaw_rate_target = 0.0f;
    turn = 0.0f;

    enable = false;

    chassis.stop();
  }

  // 设置图像输入
  void set_line(float line_error, uint8 line_valid)
  {
    this->line_error = line_error;
    this->line_valid = (line_valid != 0);
  }

  // 更新陀螺仪
  void update_gyro()
  {
    gyro.update();
  }

  // 设置速度规划参数
  void set_speed_plan(float base_speed_target,
                      float base_speed_min,
                      float turn_slow_gain,
                      float speed_ramp)
  {
    this->base_speed_target = base_speed_target;
    this->base_speed_min = base_speed_min;
    this->turn_slow_gain = turn_slow_gain;
    this->speed_ramp = speed_ramp;
  }

  // 启动
  void start()
  {
    enable = true;
  }

  // 停止
  void stop()
  {
    enable = false;
    yaw_rate_target = 0.0f;
    turn = 0.0f;
    base_speed = 0.0f;

    steer_ctrl.clear();
    yaw_ctrl.clear();
    chassis.stop();
  }

  // 清零
  void clear()
  {
    line_error = 0.0f;
    line_valid = false;

    base_speed = 0.0f;
    yaw_rate_target = 0.0f;
    turn = 0.0f;
    enable = false;

    steer_ctrl.clear();
    yaw_ctrl.clear();
    gyro.clear();
    chassis.clear();
  }

  // 获取目标角速度
  float get_yaw_rate_target() const
  {
    return yaw_rate_target;
  }

  // 获取实际角速度
  float get_yaw_rate_current() const
  {
    return gyro.get_yaw_rate();
  }

  // 获取当前 turn
  float get_turn() const
  {
    return turn;
  }

  // 获取当前基础速度
  float get_base_speed() const
  {
    return base_speed;
  }

  // 核心控制循环
  void loop()
  {
    float base_speed_plan;
    float turn_for_speed_plan;

    // 没启动就停车
    if (! enable) {
      chassis.stop();
      return;
    }

    // 更新陀螺仪
    update_gyro();

    // 外环：中线偏差 -> 目标角速度
    steer_ctrl.run(line_error);
    yaw_rate_target = steer_ctrl.get_turn();

    // 内环：目标角速度 + 当前角速度 -> turn
    if (gyro.is_valid()) {
      yaw_ctrl.run(yaw_rate_target, gyro.get_yaw_rate());

      turn = yaw_ctrl.get_turn();
      turn_for_speed_plan = std::fabs(yaw_rate_target);
    }
    else {
      turn = yaw_rate_target;
      turn_for_speed_plan = std::fabs(turn);
    }

    // 转弯时降速
    base_speed_plan = base_speed_target - turn_slow_gain * turn_for_speed_plan;
    base_speed_plan = std::clamp(base_speed_plan, base_speed_min, base_speed_target);

    base_speed = base_speed_plan;

    // 差速解算
    drive.loop(base_speed, turn);

    // 左右轮闭环
    chassis.loop();
  }

private:
  // 外环：中线偏差 -> 目标角速度
  SteeringControl steer_ctrl{};

  // 内环：目标角速度 -> turn
  YawRateControl yaw_ctrl{};

  // 差速解算
  DiffDrive drive{};

  // 陀螺仪接口
  GyroInterface gyro{};

  // 图像输入
  float line_error = 0.0f;
  bool line_valid = false;

  // 速度规划
  float base_speed = 0.0f;
  float base_speed_target = 0.30f;
  float base_speed_min = 0.15f;
  float turn_slow_gain = 1.0f;
  float speed_ramp = 0.50f;

  // 外环输出
  float yaw_rate_target = 0.0f;

  // 内环输出
  float turn = 0.0f;

  // 启停
  bool enable = false;

  zf_driver_pit timer{};
};

static ControlManager control_manager{};

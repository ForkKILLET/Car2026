#pragma once

#include <algorithm>
#include <cmath>

#include "zf_common_typedef.hpp"

#include "chassis_control.hpp"
#include "control_params.hpp"
#include "debug/data_report.hpp"
#include "diff_drive.hpp"
#include "gyro_interface.hpp"
#include "steering_control.hpp"
#include "vision.hpp"
#include "yaw_rate_control.hpp"

class CtrlManager {
public:
  // 设置图像输入
  void set_line(float line_error)
  {
    this->line_error = line_error;
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

  void loop()
  {
    float base_speed_plan;

    // 没启动就停车
    if (! enable) {
      chassis.stop();
      return;
    }

    // 更新陀螺仪
    // update_gyro();

    bool steering_updated = false, yaw_rate_updated = true;

    // 递增更新计数器
    seq++;

    // 外环：中线偏差 -> 目标角速度
    if (seq == g_params.steering.period_ratio) {
      steering_updated = true;
      steer_ctrl.run(line_error);
      yaw_rate_target = steer_ctrl.get_turn();
    }

    // 中环：目标角速度 + 当前角速度 -> turn
    if (gyro.is_valid()) {
      yaw_ctrl.run(yaw_rate_target, gyro.get_yaw_rate());
      turn = yaw_ctrl.get_turn();
    }
    else {
      turn = yaw_rate_target;
    }

    // 转弯时降速
    base_speed_plan = base_speed_target - turn_slow_gain * std::fabs(turn);
    base_speed_plan = std::max(base_speed_plan, base_speed_min);

    base_speed = base_speed_plan;

    // 差速解算
    drive.loop(base_speed, turn);

    // 内环：速度环
    chassis.loop();

    // 更新次数到达外环周期时，重置计数器
    if (seq == g_params.steering.period_ratio)
      seq = 0;

    fp_log("Ctrl: line_error = %.3f, update = %d%d1, turn = %.3f, target_l = %.3f, "
           "target_r = %.3f, curr_l = %.3f, curr_r = %.3f\n",
           line_error,
           (int) steering_updated,
           (int) yaw_rate_updated,
           turn,
           drive.get_target_l(),
           drive.get_target_r(),
           encoder_sys.get_speed_l(),
           encoder_sys.get_speed_r());

    if (g_params.debug.enable_data_report) {
      DebugDataReport::instance().send_data({line_error * g_params.steering.turn_ratio,
                                             turn,
                                             drive.get_target_l(),
                                             drive.get_target_r(),
                                             encoder_sys.get_speed_l(),
                                             encoder_sys.get_speed_r()});
    }
  }

  static void timer_callback()
  {
    instance().loop();
  }

  static CtrlManager &instance()
  {
    static CtrlManager ctrl_manager{};
    return ctrl_manager;
  }

  static void setup_timer()
  {
    static zf_driver_pit timer{};
    timer.init_ms(g_params.timer.control_period_ms, timer_callback);
  }

  // 外环：中线偏差 -> 目标角速度
  SteeringControl steer_ctrl{};

  // 内环：目标角速度 -> turn
  YawRateControl yaw_ctrl{};

  // 差速解算
  DiffDrive drive{};

  // 陀螺仪接口
  GyroInterface gyro{};

  EncoderSys &encoder_sys = EncoderSys::instance();

private:
  // 初始化
  CtrlManager()
  {
    auto &p = g_params.speed_plan;

    line_error = 0.0f;
    line_valid = false;

    base_speed = 0.0f;
    base_speed_target = p.base_speed_target;
    base_speed_min = p.base_speed_min;
    turn_slow_gain = p.turn_slow_gain;
    speed_ramp = p.speed_ramp;

    yaw_rate_target = 0.0f;
    turn = 0.0f;

    enable = false;

    chassis.stop();
  }

  // 图像输入
  float line_error = 0.0f;
  bool line_valid = false;

  // 速度规划
  float base_speed;
  float base_speed_target;
  float base_speed_min;
  float turn_slow_gain;
  float speed_ramp;

  // 外环输出
  float yaw_rate_target = 0.0f;

  // 内环输出
  float turn = 0.0f;

  // 更新次数
  uint8 seq = 0;

  // 启停
  bool enable = false;
};

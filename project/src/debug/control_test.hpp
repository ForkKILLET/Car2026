#pragma once

#include <cstdio>

#include "zf_common_typedef.hpp"
#include "zf_driver_pit.hpp"

#include "chassis_control.hpp"
#include "control_debug.hpp"
#include "control_manager.hpp"
#include "control_params.hpp"
#include "diff_drive.hpp"

enum class DebugMode {
  Stop,
  LeftSpeed,
  RightSpeed,
  DualSpeed,
  Diff,
};

class ControlTest {
public:
  ControlTest()
  {
    auto &p = g_params.speed_plan;

    ctrl.set_speed_plan(p.base_speed_target, p.base_speed_min, p.turn_slow_gain, p.speed_ramp);

    target_l = 0.0f;
    target_r = 0.0f;

    base_speed = 0.0f;
    turn = 0.0f;

    line_error = 0.0f;
    line_valid = false;
  }

  // 设置模式
  void set_mode(DebugMode mode)
  {
    this->mode = mode;
  }

  // 设置左右轮目标速度
  void set_target_speed(float target_l, float target_r)
  {
    this->target_l = target_l;
    this->target_r = target_r;
  }

  // 设置差速测试量
  void set_diff(float base_speed, float turn)
  {
    this->base_speed = base_speed;
    this->turn = turn;
  }

  // 设置伪偏差输入
  void set_line(float line_error, uint8 line_valid)
  {
    this->line_error = line_error;
    this->line_valid = (line_valid != 0);
  }

  // 停止
  void stop()
  {
    mode = DebugMode::Stop;

    target_l = 0.0f;
    target_r = 0.0f;

    base_speed = 0.0f;
    turn = 0.0f;

    line_error = 0.0f;
    line_valid = false;

    ctrl.stop();
    chassis.stop();
  }

  static ControlTest &instance()
  {
    static ControlTest test{};
    return test;
  }

  static void timer_callback(void)
  {
    auto &test = instance();
    test.control_loop();
    test.print_loop();
  }

  static void setup_timer()
  {
    static zf_driver_pit timer{};
    timer.init_ms(g_params.timer.control_period_ms, timer_callback);
  }

private:
  void control_loop()
  {
    switch (mode) {
    case DebugMode::Stop:
      ctrl.stop();
      chassis.stop();
      break;

    case DebugMode::LeftSpeed:
      chassis.set_target_speed(target_l, 0.0f);
      chassis.loop();
      break;

    case DebugMode::RightSpeed:
      chassis.set_target_speed(0.0f, target_r);
      chassis.loop();
      break;

    case DebugMode::DualSpeed:
      chassis.set_target_speed(target_l, target_r);
      chassis.loop();
      break;

    case DebugMode::Diff:
      drive.loop(base_speed, turn);
      chassis.loop();
      break;

    default:
      ctrl.stop();
      chassis.stop();
      break;
    }
  }

  void print_loop()
  {
    switch (mode) {
    case DebugMode::Stop: {
      ctrl_debug.print_stop();
      break;
    }

    case DebugMode::LeftSpeed: {
      ctrl_debug.print_left_speed(target_l);
      break;
    }

    case DebugMode::RightSpeed: {
      ctrl_debug.print_right_speed(target_r);
      break;
    }

    case DebugMode::DualSpeed: {
      ctrl_debug.print_dual_speed(target_l, target_r);
      break;
    }

    case DebugMode::Diff: {
      ctrl_debug.print_diff_test(drive, base_speed, turn);
      break;
    }

    default:
      break;
    }
  }

  zf_driver_pit control_timer{};
  zf_driver_pit print_timer{};

  DiffDrive drive{};
  CtrlManager &ctrl = CtrlManager::instance();

  DebugMode mode = DebugMode::Stop;

  float target_l = 0.0f;
  float target_r = 0.0f;

  float base_speed = 0.0f;
  float turn = 0.0f;

  float line_error = 0.0f;
  bool line_valid = false;

  int control_period_ms = 20;
  int print_period_ms = 200;
};

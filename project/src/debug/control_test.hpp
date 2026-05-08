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
  STOP,
  LEFT_SPEED,
  RIGHT_SPEED,
  DUAL_SPEED,
  DIFF_TEST,
};

static inline void control_test_control_callback(void);
static inline void control_test_print_callback(void);

class ControlTest {
public:
  void init()
  {
    car_ctrl.init(params);
    car_drive.init(params.diff_drive);

    car_ctrl.set_speed_plan(params.speed_plan.base_speed_target,
                            params.speed_plan.base_speed_min,
                            params.speed_plan.turn_slow_gain,
                            params.speed_plan.speed_ramp);

    target_l = 0.0f;
    target_r = 0.0f;

    base_speed = 0.0f;
    turn = 0.0f;

    line_error = 0.0f;
    line_valid = false;
  }

  void start()
  {
    control_timer.init_ms(control_period_ms, control_test_control_callback);
    print_timer.init_ms(print_period_ms, control_test_print_callback);
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
    mode = DebugMode::STOP;

    target_l = 0.0f;
    target_r = 0.0f;

    base_speed = 0.0f;
    turn = 0.0f;

    line_error = 0.0f;
    line_valid = false;

    car_ctrl.stop();
    chassis.stop();
  }

private:
  friend void control_test_control_callback(void);
  friend void control_test_print_callback(void);

  void control_callback()
  {
    switch (mode) {
    case DebugMode::STOP:
      car_ctrl.stop();
      chassis.stop();
      break;

    case DebugMode::LEFT_SPEED:
      chassis.set_target_speed(target_l, 0.0f);
      chassis.loop();
      break;

    case DebugMode::RIGHT_SPEED:
      chassis.set_target_speed(0.0f, target_r);
      chassis.loop();
      break;

    case DebugMode::DUAL_SPEED:
      chassis.set_target_speed(target_l, target_r);
      chassis.loop();
      break;

    case DebugMode::DIFF_TEST:
      car_drive.loop(base_speed, turn);
      chassis.loop();
      break;

    default:
      car_ctrl.stop();
      chassis.stop();
      break;
    }
  }

  void print_callback()
  {
    switch (mode) {
    case DebugMode::STOP: {
      ctrl_debug.print_stop();
      break;
    }

    case DebugMode::LEFT_SPEED: {
      ctrl_debug.print_left_speed(target_l);
      break;
    }

    case DebugMode::RIGHT_SPEED: {
      ctrl_debug.print_right_speed(target_r);
      break;
    }

    case DebugMode::DUAL_SPEED: {
      ctrl_debug.print_dual_speed(target_l, target_r);
      break;
    }

    case DebugMode::DIFF_TEST: {
      ctrl_debug.print_diff_test(car_drive, base_speed, turn);
      break;
    }

    default:
      break;
    }
  }

  zf_driver_pit control_timer{};
  zf_driver_pit print_timer{};

  DiffDrive car_drive{};
  ControlManager car_ctrl{};

  DebugMode mode = DebugMode::STOP;

  float target_l = 0.0f;
  float target_r = 0.0f;

  float base_speed = 0.0f;
  float turn = 0.0f;

  float line_error = 0.0f;
  bool line_valid = false;

  int control_period_ms = 20;
  int print_period_ms = 200;
};

static ControlTest dbg{};

// 控制定时器回调
static inline void control_test_control_callback(void)
{
  dbg.control_callback();
}

// 打印定时器回调
static inline void control_test_print_callback(void)
{
  dbg.print_callback();
}

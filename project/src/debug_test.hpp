#pragma once

#include <cstdio>
#include "zf_common_typedef.hpp"
#include "zf_driver_pit.hpp"
#include "control_params.hpp"
#include "control_debug.hpp"
#include "chassis_control.hpp"
#include "diff_drive.hpp"
#include "control_manager.hpp"

enum debug_mode
{
  debug_mode_stop = 0,
  debug_mode_left_speed,
  debug_mode_right_speed,
  debug_mode_dual_speed,
  debug_mode_diff_test,
  debug_mode_line_fake
};

struct debug_test
{
  zf_driver_pit control_timer{};
  zf_driver_pit print_timer{};

  diff_drive car_drive;
  control_manager car_ctrl;

  debug_mode mode = debug_mode_stop;

  float target_l = 0.0f;
  float target_r = 0.0f;

  float base_speed = 0.0f;
  float turn = 0.0f;

  float line_error = 0.0f;
  bool line_valid = false;

  int control_period_ms = 20;
  int print_period_ms = 200;
};

static debug_test dbg{};

// 控制定时器回调
static inline void debug_test_control_callback(void)
{
  switch (dbg.mode)
  {
  case debug_mode_stop:
  {
    control_manager_stop(dbg.car_ctrl);
    chassis_stop();
    break;
  }

  case debug_mode_left_speed:
  {
    chassis_set_target_speed(dbg.target_l, 0.0f);
    chassis_control_loop();
    break;
  }

  case debug_mode_right_speed:
  {
    chassis_set_target_speed(0.0f, dbg.target_r);
    chassis_control_loop();
    break;
  }

  case debug_mode_dual_speed:
  {
    chassis_set_target_speed(dbg.target_l, dbg.target_r);
    chassis_control_loop();
    break;
  }

  case debug_mode_diff_test:
  {
    diff_drive_loop(dbg.car_drive, dbg.base_speed, dbg.turn);
    chassis_control_loop();
    break;
  }

  case debug_mode_line_fake:
  {
    control_manager_start(dbg.car_ctrl);
    control_manager_set_line(dbg.car_ctrl, dbg.line_error, dbg.line_valid);
    control_manager_loop(dbg.car_ctrl, (float) dbg.control_period_ms / 1000.0f);
    break;
  }

  default:
  {
    control_manager_stop(dbg.car_ctrl);
    chassis_stop();
    break;
  }
  }
}

// 打印定时器回调
static inline void debug_test_print_callback(void)
{
  switch (dbg.mode)
  {
  case debug_mode_stop:
  {
    control_debug_print_stop();
    break;
  }

  case debug_mode_left_speed:
  {
    control_debug_print_left_speed(dbg.target_l);
    break;
  }

  case debug_mode_right_speed:
  {
    control_debug_print_right_speed(dbg.target_r);
    break;
  }

  case debug_mode_dual_speed:
  {
    control_debug_print_dual_speed(dbg.target_l, dbg.target_r);
    break;
  }

  case debug_mode_diff_test:
  {
    control_debug_print_diff_test(dbg.car_drive, dbg.base_speed, dbg.turn);
    break;
  }

  case debug_mode_line_fake:
  {
    control_debug_print_line_fake(dbg.car_ctrl, dbg.line_error, dbg.line_valid);
    break;
  }

  default:
    break;
  }
}

// 初始化
static inline void debug_test_init()
{
  control_manager_init(dbg.car_ctrl);
  diff_drive_init(dbg.car_drive);

  control_manager_set_speed_plan(dbg.car_ctrl,
                                 params.speed_plan.base_speed_target,
                                 params.speed_plan.base_speed_min,
                                 params.speed_plan.turn_slow_gain,
                                 params.speed_plan.speed_ramp);

  dbg.target_l = 0.0f;
  dbg.target_r = 0.0f;

  dbg.base_speed = 0.0f;
  dbg.turn = 0.0f;

  dbg.line_error = 0.0f;
  dbg.line_valid = 0;

}

static inline void debug_test_start()
{
  dbg.control_timer.init_ms(dbg.control_period_ms, debug_test_control_callback);
  dbg.print_timer.init_ms(dbg.print_period_ms, debug_test_print_callback);
}

// 设置模式
static inline void debug_test_set_mode(debug_mode mode)
{
  dbg.mode = mode;
}

// 设置左右轮目标速度
static inline void debug_test_set_target_speed(float target_l,
                                               float target_r)
{
  dbg.target_l = target_l;
  dbg.target_r = target_r;
}

// 设置差速测试量
static inline void debug_test_set_diff(float base_speed,
                                       float turn)
{
  dbg.base_speed = base_speed;
  dbg.turn = turn;
}

// 设置伪偏差输入
static inline void debug_test_set_line(float line_error,
                                       uint8 line_valid)
{
  dbg.line_error = line_error;
  dbg.line_valid = line_valid;
}

// 停止
static inline void debug_test_stop()
{
  dbg.mode = debug_mode_stop;

  dbg.target_l = 0.0f;
  dbg.target_r = 0.0f;

  dbg.base_speed = 0.0f;
  dbg.turn = 0.0f;

  dbg.line_error = 0.0f;
  dbg.line_valid = 0;

  control_manager_stop(dbg.car_ctrl);
  chassis_stop();
}

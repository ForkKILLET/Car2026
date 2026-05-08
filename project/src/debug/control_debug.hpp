#pragma once

#include <cstdio>
#include <zf_common_typedef.hpp>

#include "chassis_control.hpp"
#include "control_manager.hpp"
#include "diff_drive.hpp"

class ControlDebug {
public:
  void print_stop() const
  {
    std::printf("[STOP ] speed_l=%.3f speed_r=%.3f speed_car=%.3f\n",
                chassis.get_speed_l(),
                chassis.get_speed_r(),
                chassis.get_speed_car());
  }

  void print_left_speed(float target_l) const
  {
    std::printf("[LEFT ] target_l=%.3f speed_l=%.3f pwm=%d\n",
                target_l,
                chassis.get_speed_l(),
                chassis.get_pwm_l());
  }

  void print_right_speed(float target_r) const
  {
    std::printf("[RIGHT] target_r=%.3f speed_r=%.3f pwm=%d\n",
                target_r,
                chassis.get_speed_r(),
                chassis.get_pwm_r());
  }

  void print_dual_speed(float target_l, float target_r) const
  {
    std::printf("[DUAL ] target_l=%.3f target_r=%.3f speed_l=%.3f speed_r=%.3f "
                "pwm_l=%d pwm_r=%d\r\n",
                target_l,
                target_r,
                chassis.get_speed_l(),
                chassis.get_speed_r(),
                chassis.get_pwm_l(),
                chassis.get_pwm_r());
  }

  void print_diff_test(const DiffDrive &drive, float base_speed, float turn) const
  {
    std::printf("[DIFF ] base=%.3f turn=%.3f target_l=%.3f target_r=%.3f "
                "speed_l=%.3f speed_r=%.3f pwm_l=%d pwm_r=%d\r\n",
                base_speed,
                turn,
                drive.get_target_l(),
                drive.get_target_r(),
                chassis.get_speed_l(),
                chassis.get_speed_r(),
                chassis.get_pwm_l(),
                chassis.get_pwm_r());
  }
};

static ControlDebug ctrl_debug{};

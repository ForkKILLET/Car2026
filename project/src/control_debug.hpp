#pragma once

#include <cstdio>
#include "zf_common_typedef.hpp"
#include "chassis_control.hpp"
#include "control_manager.hpp"
#include "diff_drive.hpp"


static inline void control_debug_print_stop(void)
{
    std::printf("[STOP] speed_l=%.3f speed_r=%.3f speed_car=%.3f yaw=%.3f\r\n",
                chassis_get_speed_l(),
                chassis_get_speed_r(),
                chassis_get_speed_car(),
                chassis_get_yaw_rate());
}


static inline void control_debug_print_left_speed(float target_l)
{
    std::printf("[LEFT ] target_l=%.3f speed_l=%.3f speed_r=%.3f yaw=%.3f\r\n",
                target_l,
                chassis_get_speed_l(),
                chassis_get_speed_r(),
                chassis_get_yaw_rate());
}


static inline void control_debug_print_right_speed(float target_r)
{
    std::printf("[RIGHT] target_r=%.3f speed_l=%.3f speed_r=%.3f yaw=%.3f\r\n",
                target_r,
                chassis_get_speed_l(),
                chassis_get_speed_r(),
                chassis_get_yaw_rate());
}


static inline void control_debug_print_dual_speed(float target_l, float target_r)
{
    std::printf("[DUAL ] target_l=%.3f target_r=%.3f speed_l=%.3f speed_r=%.3f yaw=%.3f\r\n",
                target_l,
                target_r,
                chassis_get_speed_l(),
                chassis_get_speed_r(),
                chassis_get_yaw_rate());
}


static inline void control_debug_print_diff_test(const diff_drive& drive,
                                                 float base_speed,
                                                 float turn)
{
    std::printf("[DIFF ] base=%.3f turn=%.3f target_l=%.3f target_r=%.3f speed_l=%.3f speed_r=%.3f yaw=%.3f\r\n",
                base_speed,
                turn,
                drive.target_l,
                drive.target_r,
                chassis_get_speed_l(),
                chassis_get_speed_r(),
                chassis_get_yaw_rate());
}


static inline void control_debug_print_line_fake(control_manager& ctrl,
                                                 float line_error,
                                                 uint8 line_valid)
{
    std::printf("[LINE ] line_error=%.3f valid=%d turn=%.3f base=%.3f speed_l=%.3f speed_r=%.3f yaw=%.3f\r\n",
                line_error,
                (int)line_valid,
                control_manager_get_turn(ctrl),
                control_manager_get_base_speed(ctrl),
                chassis_get_speed_l(),
                chassis_get_speed_r(),
                chassis_get_yaw_rate());
}

#pragma once

#include "zf_common_typedef.hpp"
#include "steering_control.hpp"
#include "diff_drive.hpp"
#include "chassis_control.hpp"
#include <cmath>

// line_error -> steering_control -> turn
// base_speed + turn -> diff_drive -> target_l / target_r
// target_l / target_r -> chassis_control -> 左右轮闭环
// 丢线时停车

struct control_manager
{

    steering_control steer_ctrl;

    diff_drive drive;

    float line_error;     // 当前中线偏差
    uint8 line_valid;     // 当前偏差是否有效，1有效，0无效

    float base_speed;           // 当前实际基础速度
    float base_speed_target;    // 目标基础速度（直道速度）
    float base_speed_min;       // 最小基础速度
    float turn_slow_gain;       // 降速系数
    float speed_ramp;           // 基础速度变化斜率（m/s^2）

    float turn;

    uint8 enable;

    control_manager()
    {
        line_error = 0.0f;
        line_valid = 0;

        base_speed = 0.0f;
        base_speed_target = 0.30f;
        base_speed_min = 0.15f;
        turn_slow_gain = 1.0f;
        speed_ramp = 0.50f;

        turn = 0.0f;
        enable = 0;
    }
};


// 初始化
static inline void control_manager_init(control_manager& ctrl)
{

    chassis_control_init();

    // 方向环初始化
    steering_control_init(ctrl.steer_ctrl,
                          0.8f,    // kp
                          0.2f,    // kd
                          0.15f,   // turn_limit
                          0.0f);   // deadband

    // 差速解算初始化
    diff_drive_init(ctrl.drive, 0.5f);

    ctrl.line_error = 0.0f;
    ctrl.line_valid = 0;

    ctrl.base_speed = 0.0f;
    ctrl.base_speed_target = 0.30f;
    ctrl.base_speed_min = 0.15f;
    ctrl.turn_slow_gain = 1.0f;
    ctrl.speed_ramp = 0.50f;

    ctrl.turn = 0.0f;
    ctrl.enable = 0;

    chassis_stop();
}


// 图像输入
static inline void control_manager_set_line(control_manager& ctrl,
                                            float line_error,
                                            uint8 line_valid)
{
    ctrl.line_error = line_error;
    ctrl.line_valid = line_valid;
}


// 设置速度规划参数
static inline void control_manager_set_speed_plan(control_manager& ctrl,
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
static inline void control_manager_start(control_manager& ctrl)
{
    ctrl.enable = 1;
}


// 停止
static inline void control_manager_stop(control_manager& ctrl)
{
    ctrl.enable = 0;
    ctrl.turn = 0.0f;
    ctrl.base_speed = 0.0f;

    steering_control_clear(ctrl.steer_ctrl);
    chassis_stop();
}


// 清零
static inline void control_manager_clear(control_manager& ctrl)
{
    ctrl.line_error = 0.0f;
    ctrl.line_valid = 0;

    ctrl.base_speed = 0.0f;
    ctrl.turn = 0.0f;
    ctrl.enable = 0;

    steering_control_clear(ctrl.steer_ctrl);
    chassis_clear();
}


// 获取turn
static inline float control_manager_get_turn(control_manager& ctrl)
{
    return ctrl.turn;
}


// 获取基础速度
static inline float control_manager_get_base_speed(control_manager& ctrl)
{
    return ctrl.base_speed;
}


// 核心控制循环
static inline void control_manager_loop(control_manager& ctrl, float dt_s)
{
    float base_speed_plan;
    float max_step;

    // 没启动，停车
    if (ctrl.enable == 0)
    {
        chassis_stop();
        return;
    }

    // 丢线，停车
    if (ctrl.line_valid == 0)
    {
        ctrl.turn = 0.0f;
        ctrl.base_speed = 0.0f;

        steering_control_clear(ctrl.steer_ctrl);
        chassis_stop();
        return;
    }

    // 中线偏差 -> turn
    steering_control_run(ctrl.steer_ctrl, ctrl.line_error);
    ctrl.turn = ctrl.steer_ctrl.turn;

    // 简单速度规划：转弯时降速
    base_speed_plan = ctrl.base_speed_target - ctrl.turn_slow_gain * std::fabs(ctrl.turn);

    if (base_speed_plan < ctrl.base_speed_min)
    {
        base_speed_plan = ctrl.base_speed_min;
    }

    if (base_speed_plan > ctrl.base_speed_target)
    {
        base_speed_plan = ctrl.base_speed_target;
    }

    // 基础速度斜坡
    if (dt_s > 0.0f)
    {
        max_step = ctrl.speed_ramp * dt_s;

        if (ctrl.base_speed < base_speed_plan)
        {
            ctrl.base_speed += max_step;
            if (ctrl.base_speed > base_speed_plan)
            {
                ctrl.base_speed = base_speed_plan;
            }
        }
        else if (ctrl.base_speed > base_speed_plan)
        {
            ctrl.base_speed -= max_step;
            if (ctrl.base_speed < base_speed_plan)
            {
                ctrl.base_speed = base_speed_plan;
            }
        }
    }
    else
    {
        ctrl.base_speed = base_speed_plan;
    }

    diff_drive_run(ctrl.drive, ctrl.base_speed, ctrl.turn);

    chassis_control_loop();
}

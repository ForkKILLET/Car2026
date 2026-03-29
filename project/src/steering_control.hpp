#pragma once
#include "zf_common_typedef.hpp"
#include <cmath>

// 输入中线偏差 line_error
// 输出转向修正量 turn

struct steering_control
{
    // pd 参数
    float kp;
    float kd;

    // 当前误差、上一次误差
    float error;
    float error_last;

    // 输出 turn
    float turn;

    // 输出限幅
    float turn_limit;

    // 小误差死区
    float deadband;

    steering_control()
    {
        kp = 0.0f;
        kd = 0.0f;

        error = 0.0f;
        error_last = 0.0f;

        turn = 0.0f;
        turn_limit = 1.0f;

        deadband = 0.0f;
    }
};


// 初始化
static inline void steering_control_init(steering_control& ctrl,
                                         float kp,
                                         float kd,
                                         float turn_limit,
                                         float deadband)
{
    ctrl.kp = kp;
    ctrl.kd = kd;

    ctrl.error = 0.0f;
    ctrl.error_last = 0.0f;

    ctrl.turn = 0.0f;
    ctrl.turn_limit = turn_limit;

    ctrl.deadband = deadband;
}


// 清零
static inline void steering_control_clear(steering_control& ctrl)
{
    ctrl.error = 0.0f;
    ctrl.error_last = 0.0f;
    ctrl.turn = 0.0f;
}


// 更新方向控制
static inline void steering_control_update(steering_control& ctrl,
                                           float target_error,
                                           float current_error)
{
    float delta_error;

    ctrl.error = target_error - current_error;

    // 小误差死区
    if (std::fabs(ctrl.error) < ctrl.deadband)
    {
        ctrl.error = 0.0f;
    }

    delta_error = ctrl.error - ctrl.error_last;

    // PD 输出
    ctrl.turn = ctrl.kp * ctrl.error + ctrl.kd * delta_error;

    // 输出限幅
    if (ctrl.turn > ctrl.turn_limit)
    {
        ctrl.turn = ctrl.turn_limit;
    }
    else if (ctrl.turn < -ctrl.turn_limit)
    {
        ctrl.turn = -ctrl.turn_limit;
    }

    ctrl.error_last = ctrl.error;
}


// 输入当前偏差，目标默认 0
static inline void steering_control_run(steering_control& ctrl,
                                        float line_error)
{
    steering_control_update(ctrl, 0.0f, line_error);
}

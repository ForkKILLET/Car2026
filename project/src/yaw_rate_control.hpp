#pragma once

#include "pid.hpp"
#include "zf_common_typedef.hpp"

struct yaw_rate_control
{
    PID pid;

    float yaw_rate_target;
    float yaw_rate_current;

    float turn;
    float turn_limit;

    yaw_rate_control()
    {
        yaw_rate_target = 0.0f;
        yaw_rate_current = 0.0f;

        turn = 0.0f;
        turn_limit = 1.0f;
    }
};


// 初始化
static inline void yaw_rate_control_init(yaw_rate_control& ctrl,
                                         float kp,
                                         float ki,
                                         float kd,
                                         float error_limit,
                                         float turn_limit)
{
    PID_set(ctrl.pid, PID_POS, kp, ki, kd);
    PID_limit(ctrl.pid, error_limit, turn_limit);

    ctrl.yaw_rate_target = 0.0f;
    ctrl.yaw_rate_current = 0.0f;

    ctrl.turn = 0.0f;
    ctrl.turn_limit = turn_limit;
}


// 清零
static inline void yaw_rate_control_clear(yaw_rate_control& ctrl)
{
    ctrl.yaw_rate_target = 0.0f;
    ctrl.yaw_rate_current = 0.0f;
    ctrl.turn = 0.0f;

    PID_clean(ctrl.pid);
}


// 更新
static inline void yaw_rate_control_update(yaw_rate_control& ctrl,
                                           float yaw_rate_target,
                                           float yaw_rate_current)
{
    ctrl.yaw_rate_target = yaw_rate_target;
    ctrl.yaw_rate_current = yaw_rate_current;

    PID_position(ctrl.pid,
                 ctrl.yaw_rate_target,
                 ctrl.yaw_rate_current);

    ctrl.turn = ctrl.pid.output_pos;

    if (ctrl.turn > ctrl.turn_limit)
    {
        ctrl.turn = ctrl.turn_limit;
    }
    else if (ctrl.turn < -ctrl.turn_limit)
    {
        ctrl.turn = -ctrl.turn_limit;
    }
}


// 一步运行
static inline void yaw_rate_control_run(yaw_rate_control& ctrl,
                                        float yaw_rate_target,
                                        float yaw_rate_current)
{
    yaw_rate_control_update(ctrl, yaw_rate_target, yaw_rate_current);
}

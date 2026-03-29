#pragma once
#include "pid.hpp"
#include "motor.hpp"
#include "zf_common_typedef.hpp"

// 单个电机速度闭环控制
// 目标速度 - 当前速度 -> 增量 pid -> pwm 输出

struct motor_speed_control
{
    // pid 控制器
    PID pid;

    // 目标速度（m/s）
    float target_speed;

    // 当前速度（m/s）
    float current_speed;

    // 输出 pwm
    int16 pwm_out;

    // pwm 限幅
    int16 pwm_limit;

    // 构造默认清零
    motor_speed_control()
    {
        target_speed = 0.0f;
        current_speed = 0.0f;
        pwm_out = 0;
        pwm_limit = 1000;
    }
};

// 初始化
static inline void motor_speed_init(motor_speed_control& ctrl,
                                    float kp,
                                    float ki,
                                    float kd,
                                    float error_limit,
                                    int16 pwm_limit)
{
    PID_set(ctrl.pid, PID_INC, kp, ki, kd);
    PID_limit(ctrl.pid, error_limit, (float)pwm_limit);

    ctrl.target_speed = 0.0f;
    ctrl.current_speed = 0.0f;
    ctrl.pwm_out = 0;
    ctrl.pwm_limit = pwm_limit;
}


// 设置目标速度
static inline void motor_speed_set_target(motor_speed_control& ctrl,
                                          float target_speed)
{
    ctrl.target_speed = target_speed;
}

// 设置当前速度
static inline void motor_speed_set_current(motor_speed_control& ctrl,
                                           float current_speed)
{
    ctrl.current_speed = current_speed;
}


// 执行一次闭环更新
static inline void motor_speed_update(motor_speed_control& ctrl,
                                      fp_motor& motor)
{
    // 增量 pid
    PID_increment(ctrl.pid, ctrl.target_speed, ctrl.current_speed);

    ctrl.pwm_out = (int16)(ctrl.pid.output_inc);

    //限幅
    if (ctrl.pwm_out > ctrl.pwm_limit)
    {
        ctrl.pwm_out = ctrl.pwm_limit;
    }
    else if (ctrl.pwm_out < -ctrl.pwm_limit)
    {
        ctrl.pwm_out = -ctrl.pwm_limit;
    }

    // 输出到电机
    motor.set_duty(ctrl.pwm_out);
}


// 停止闭环控制
static inline void motor_speed_stop(motor_speed_control& ctrl,
                                    fp_motor& motor)
{
    ctrl.target_speed = 0.0f;
    ctrl.current_speed = 0.0f;
    ctrl.pwm_out = 0;

    PID_clean(ctrl.pid);
    motor.set_duty(0);
}

#pragma once

#include "encoder_speed_system.hpp"
#include "motor.hpp"
#include "motor_speed_control.hpp"

// 功能：
// 1初始化左右编码器测速
// 2初始化左右电机
// 3初始化左右轮增量 pid
// 4提供左右轮目标速度设置接口
// 5提供左右轮闭环更新接口


// 路径
static encoder_speed_system encoder_sys("/encoder_l",
                                    "/encoder_r");

static fp_motor motor_l("/pwm_l", "/dir_l");
static fp_motor motor_r("/pwm_r", "/dir_r");

static motor_speed_control motor_l_ctrl;
static motor_speed_control motor_r_ctrl;


// 初始化
static inline void chassis_control_init(void)
{
    float k;

    // 1907 count -> 0.201 m
    k = 0.201f / 1907.0f;

    // 编码器测速初始化
    encoder_speed_init(encoder_sys,
                       k,
                       k,
                       0.2f,   // 轮距
                       0.5f,    // 滤波系数
                       20);     // 定时器周期 20ms

    // 电机初始化
    motor_l.init();
    motor_r.init();

    // 左右轮增量 pid 初始化
    motor_speed_init(motor_l_ctrl, 50.0f, 5.0f, 1.0f, 1000.0f, 1000);
    motor_speed_init(motor_r_ctrl, 50.0f, 5.0f, 1.0f, 1000.0f, 1000);

    // 初始目标速度设为 0
    motor_speed_set_target(motor_l_ctrl, 0.0f);
    motor_speed_set_target(motor_r_ctrl, 0.0f);
}



// 设置左右轮目标速度
// 单位:m/s
static inline void chassis_set_target_speed(float target_l, float target_r)
{
    motor_speed_set_target(motor_l_ctrl, target_l);
    motor_speed_set_target(motor_r_ctrl, target_r);
}


// 获取左右轮当前速度
static inline float chassis_get_speed_l(void)
{
    return encoder_sys.speed_l;
}

static inline float chassis_get_speed_r(void)
{
    return encoder_sys.speed_r;
}


// 获取车体线速度与角速度
static inline float chassis_get_speed_car(void)
{
    return encoder_sys.speed_car;
}

static inline float chassis_get_yaw_rate(void)
{
    return encoder_sys.yaw_rate;
}


// 闭环更新
static inline void chassis_control_loop(void)
{
    // 左轮
    motor_speed_set_current(motor_l_ctrl, encoder_sys.speed_l);
    motor_speed_update(motor_l_ctrl, motor_l);

    // 右轮
    motor_speed_set_current(motor_r_ctrl, encoder_sys.speed_r);
    motor_speed_update(motor_r_ctrl, motor_r);
}


// 停车
static inline void chassis_stop(void)
{
    motor_speed_stop(motor_l_ctrl, motor_l);
    motor_speed_stop(motor_r_ctrl, motor_r);
}


// 清零
static inline void chassis_clear(void)
{
    encoder_speed_clear(encoder_sys);

    motor_speed_stop(motor_l_ctrl, motor_l);
    motor_speed_stop(motor_r_ctrl, motor_r);
}

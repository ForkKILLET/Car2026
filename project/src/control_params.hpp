#pragma once

#include "zf_common_typedef.hpp"

struct encoder_params
{
    float meter_per_count_l;
    float meter_per_count_r;
    float wheel_base;
    float alpha;
    int timer_ms;

    encoder_params()
    {
        meter_per_count_l = 0.201f / 1907.0f;
        meter_per_count_r = 0.201f / 1907.0f;
        wheel_base = 0.20f;
        alpha = 0.50f;
        timer_ms = 20;
    }
};

struct motor_speed_pid_params
{
    float kp;
    float ki;
    float kd;
    float error_limit;
    int16 pwm_limit;

    motor_speed_pid_params()
    {
        kp = 50.0f;
        ki = 5.0f;
        kd = 1.0f;
        error_limit = 1000.0f;
        pwm_limit = 1000;
    }
};

struct steering_params
{
    float kp;
    float kd;
    float output_limit;
    float deadband;

    steering_params()
    {
        kp = 0.8f;
        kd = 0.2f;
        output_limit = 0.60f;
        deadband = 0.0f;
    }
};

struct yaw_rate_pid_params
{
    float kp;
    float ki;
    float kd;
    float error_limit;
    float turn_limit;

    yaw_rate_pid_params()
    {
        kp = 1.2f;
        ki = 0.0f;
        kd = 0.08f;
        error_limit = 1000.0f;
        turn_limit = 0.15f;
    }
};

struct diff_drive_params
{
    float target_limit;

    diff_drive_params()
    {
        target_limit = 0.50f;
    }
};

struct speed_plan_params
{
    float base_speed_target;
    float base_speed_min;
    float turn_slow_gain;
    float speed_ramp;

    speed_plan_params()
    {
        base_speed_target = 0.30f;
        base_speed_min = 0.15f;
        turn_slow_gain = 1.0f;
        speed_ramp = 0.50f;
    }
};

struct gyro_params
{
    float gyro_scale;
    int bias_sample_count;
    int bias_sample_delay_ms;

    gyro_params()
    {
        gyro_scale = 16.4f;
        bias_sample_count = 200;
        bias_sample_delay_ms = 2;
    }
};

struct state_machine_params
{
    uint32 lost_line_limit;

    state_machine_params()
    {
        lost_line_limit = 10;
    }
};

struct control_params
{
    encoder_params encoder;
    motor_speed_pid_params motor_l_pid;
    motor_speed_pid_params motor_r_pid;
    steering_params steering;
    yaw_rate_pid_params yaw_rate;
    diff_drive_params diff_drive;
    speed_plan_params speed_plan;
    gyro_params gyro;
    state_machine_params state_machine;
};

inline control_params params;

#include "pid.hpp"

void PID_set(PIDType type, PID *pid, float kp, float ki, float kd)
{
    pid->type = type;

    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;

    pid->error_previous = 0.0f;
    pid->error_last = 0.0f;
    pid->error = 0.0f;

    pid->error_sum = 0.0f;
    pid->error_limit = 1000.0f;
    pid->error_zero_count = 0;

    pid->output_inc = 0.0f;
    pid->output_pos = 0.0f;
    pid->output_limit = 1000.0f;
}

void PID_limit(PID *pid, float error_limit, float output_limit)
{
    pid->error_limit = error_limit;
    pid->output_limit = output_limit;
}

void PID_position(PID *pid, float target, float current)
{
    float delta_error;

    pid->error = target - current;

    float deadband = 0.5f; // 死区处理，阈值可调
    if (fabs(pid->error) < deadband)
        pid->error = 0.0f;

    pid->error_sum += pid->error;

    // 若误差长时间为0，则清空积分项
    if (pid->error == 0.0f)
    {
        pid->error_zero_count++;
        if (pid->error_zero_count > 50)
        {
            pid->error_sum = 0.0f;
            pid->error_zero_count = 0;
        }
    }
    else
    {
        pid->error_zero_count = 0;
    }

    // 积分限幅
    if (pid->error_sum > pid->error_limit)
    {
        pid->error_sum = pid->error_limit;
    }
    else if (pid->error_sum < -pid->error_limit)
    {
        pid->error_sum = -pid->error_limit;
    }

    // 微分项
    delta_error = pid->error - pid->error_last;

    pid->output_pos = pid->kp * pid->error + pid->ki * pid->error_sum + pid->kd * delta_error;

    // 输出限幅
    if (pid->output_pos > pid->output_limit)
    {
        pid->output_pos = pid->output_limit;
    }
    else if (pid->output_pos < -pid->output_limit)
    {
        pid->output_pos = -pid->output_limit;
    }

    pid->error_last = pid->error;
}

void PID_increment(PID *pid, float target, float current)
{
    float delta_output;

    pid->error = target - current;


    delta_output = pid->kp * (pid->error - pid->error_last) + pid->ki * pid->error + pid->kd * (pid->error - 2.0f * pid->error_last + pid->error_previous);

    pid->output_inc += delta_output;

    // 输出限幅
    if (pid->output_inc > pid->output_limit)
    {
        pid->output_inc = pid->output_limit;
    }
    else if (pid->output_inc < -pid->output_limit)
    {
        pid->output_inc = -pid->output_limit;
    }

    pid->error_previous = pid->error_last;
    pid->error_last = pid->error;
}

void PID_clean(PID *pid)
{
    pid->error_previous = 0.0f;
    pid->error_last = 0.0f;
    pid->error = 0.0f;
    pid->error_sum = 0.0f;
    pid->error_zero_count = 0;
    pid->output_inc = 0.0f;
    pid->output_pos = 0.0f;
}
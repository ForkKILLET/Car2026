#ifndef PID_HPP
#define PID_HPP

#include "zf_common_typedef.hpp"

typedef enum _PIDType
{
    PID_POI,   // 位置式PID
    PID_INC,   // 增量式PID
} PIDType;

typedef struct _PID
{
    PIDType  type;               // PID类型

    float    kp;             
    float    ki;                 
    float    kd;                

    float    error_previous;      // 上上次误差
    float    error_last;          // 上次误差
    float    error;              // 当前误差

    float    error_sum;         
    float    error_limit;        // 积分限幅
    uint32   error_zero_count;   // 误差连续为0的计数

    float    output_pos;         //位置式pid输出
    float    output_inc;         //增量
    float    output_limit;       // 输出限幅
} PID;

// PID初始化
void PID_set(PIDType type, PID *pid, float kp, float ki, float kd);

// 设置PID限幅
void PID_limit(PID *pid, float error_limit, float output_limit);

void PID_position(PID *pid, float target, float current);

void PID_increment(PID *pid, float target, float current);

void PID_clean(PID *pid);

#endif
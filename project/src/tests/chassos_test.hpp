#pragma once

#include "chassis_control.hpp"
#include "zf_driver_pit.hpp"
#include <cstdio>

// =====================================================
// 测试说明：
// 1. 编码器测速 pit 在 chassis_control_init() 里已经启动
// 2. 这里再启动一个 control pit，固定周期调用 chassis_control_loop()
// 3. 下面提供 3 个测试启动函数：
//    chassis_test_start_left()
//    chassis_test_start_right()
//    chassis_test_start_both()
// =====================================================


// ==========================
// 控制定时器
// ==========================
static zf_driver_pit chassis_test_timer{};

// 打印计数，用来降低打印频率
static uint32 chassis_test_print_count = 0;


// ==========================
// 定时器回调
// 每 20ms 调一次控制循环
// 每 10 次打印一次，大约 200ms 打印一次
// ==========================
static inline void chassis_test_timer_callback(void)
{
    chassis_control_loop();

    chassis_test_print_count++;

    if (chassis_test_print_count >= 10)
    {
        chassis_test_print_count = 0;

        std::printf("target_l=%.3f  speed_l=%.3f  pwm_l=%d | "
                    "target_r=%.3f  speed_r=%.3f  pwm_r=%d | "
                    "speed_car=%.3f  yaw_rate=%.3f\n",
                    motor_l_ctrl.target_speed,
                    encoder_sys.speed_l,
                    motor_l_ctrl.pwm_out,
                    motor_r_ctrl.target_speed,
                    encoder_sys.speed_r,
                    motor_r_ctrl.pwm_out,
                    encoder_sys.speed_car,
                    encoder_sys.yaw_rate);
    }
}


// ==========================
// 测试初始化
// 先初始化底盘，再启动控制定时器
// ==========================
static inline void chassis_test_init(void)
{
    chassis_control_init();

    // 控制循环也按 20ms 跑一次
    chassis_test_timer.init_ms(20, chassis_test_timer_callback);
}


// ==========================
// 停止测试
// ==========================
static inline void chassis_test_stop(void)
{
    chassis_test_timer.stop();
    chassis_stop();
}


// ==========================
// 测试1：左轮单独闭环
// 右轮目标速度设为 0
// ==========================
static inline void chassis_test_start_left(float target_speed_l)
{
    chassis_set_target_speed(target_speed_l, 0.0f);

    std::printf("start left wheel test: target_l=%.3f, target_r=0.000\n",
                target_speed_l);
}


// ==========================
// 测试2：右轮单独闭环
// 左轮目标速度设为 0
// ==========================
static inline void chassis_test_start_right(float target_speed_r)
{
    chassis_set_target_speed(0.0f, target_speed_r);

    std::printf("start right wheel test: target_l=0.000, target_r=%.3f\n",
                target_speed_r);
}


// ==========================
// 测试3：左右轮同速闭环
// ==========================
static inline void chassis_test_start_both(float target_speed)
{
    chassis_set_target_speed(target_speed, target_speed);

    std::printf("start both wheel test: target_l=%.3f, target_r=%.3f\n",
                target_speed,
                target_speed);
}

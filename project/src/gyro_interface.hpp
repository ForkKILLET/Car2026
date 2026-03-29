#pragma once
#include "zf_common_typedef.hpp"


struct gyro_interface
{
    float yaw_rate;   // 陀螺仪角速度（rad/s）
    uint8 valid;      // 数据是否有效，1有效，0无效

    gyro_interface()
    {
        yaw_rate = 0.0f;
        valid = 0;
    }
};


// 初始化
static inline void gyro_interface_init(gyro_interface& gyro)
{
    gyro.yaw_rate = 0.0f;
    gyro.valid = 0;
}


// 设置当前角速度
static inline void gyro_interface_set(gyro_interface& gyro,
                                      float yaw_rate,
                                      uint8 valid)
{
    gyro.yaw_rate = yaw_rate;
    gyro.valid = valid;
}


// 清零
static inline void gyro_interface_clear(gyro_interface& gyro)
{
    gyro.yaw_rate = 0.0f;
    gyro.valid = 0;
}

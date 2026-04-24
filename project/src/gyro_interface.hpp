#pragma once

#include "zf_common_typedef.hpp"
#include "zf_device_imu.hpp"
#include <unistd.h>
#include <cmath>

struct gyro_interface
{
    zf_device_imu imu;
    imu_device_type_enum imu_type;

    int16 raw_z;
    float bias_z;
    float raw_z_corr;

    float yaw_rate_dps;
    float yaw_rate;

    float gyro_scale;

    uint8 valid;
    uint8 bias_ready;

    gyro_interface()
    {
        imu_type = DEV_NO_FIND;

        raw_z = 0;
        bias_z = 0.0f;
        raw_z_corr = 0.0f;

        yaw_rate_dps = 0.0f;
        yaw_rate = 0.0f;

        gyro_scale = 16.4f;

        valid = 0;
        bias_ready = 0;
    }
};


static inline uint8 gyro_interface_init(gyro_interface& gyro)
{
    gyro.imu_type = gyro.imu.init();

    gyro.raw_z = 0;
    gyro.bias_z = 0.0f;
    gyro.raw_z_corr = 0.0f;
    gyro.yaw_rate_dps = 0.0f;
    gyro.yaw_rate = 0.0f;
    gyro.bias_ready = 0;

    if (gyro.imu_type == DEV_NO_FIND)
    {
        gyro.valid = 0;
        return 0;
    }

    gyro.valid = 1;
    return 1;
}


static inline void gyro_interface_set_scale(gyro_interface& gyro, float gyro_scale)
{
    if (gyro_scale > 0.0f)
    {
        gyro.gyro_scale = gyro_scale;
    }
}


static inline int16 gyro_interface_read_raw_z(gyro_interface& gyro)
{
    if (gyro.valid == 0)
    {
        return 0;
    }

    return gyro.imu.get_gyro_z();
}


static inline uint8 gyro_interface_calibrate_bias(gyro_interface& gyro,
                                                  int sample_count,
                                                  int sample_delay_ms)
{
    int i;
    long sum = 0;

    if (gyro.valid == 0)
    {
        return 0;
    }

    if (sample_count <= 0)
    {
        return 0;
    }

    for (i = 0; i < sample_count; i++)
    {
        sum += gyro_interface_read_raw_z(gyro);

        if (sample_delay_ms > 0)
        {
            usleep(sample_delay_ms * 1000);
        }
    }

    gyro.bias_z = (float)sum / (float)sample_count;
    gyro.bias_ready = 1;

    return 1;
}


static inline void gyro_interface_update(gyro_interface& gyro)
{
    if (gyro.valid == 0)
    {
        gyro.raw_z = 0;
        gyro.raw_z_corr = 0.0f;
        gyro.yaw_rate_dps = 0.0f;
        gyro.yaw_rate = 0.0f;
        return;
    }

    gyro.raw_z = gyro_interface_read_raw_z(gyro);
    gyro.raw_z_corr = (float)gyro.raw_z - gyro.bias_z;

    if (gyro.gyro_scale > 0.0f)
    {
        gyro.yaw_rate_dps = gyro.raw_z_corr / gyro.gyro_scale;
    }
    else
    {
        gyro.yaw_rate_dps = 0.0f;
    }

    gyro.yaw_rate = gyro.yaw_rate_dps * 3.1415926f / 180.0f;
}


static inline void gyro_interface_clear(gyro_interface& gyro)
{
    gyro.raw_z = 0;
    gyro.raw_z_corr = 0.0f;
    gyro.yaw_rate_dps = 0.0f;
    gyro.yaw_rate = 0.0f;
}


static inline float gyro_interface_get_yaw_rate(gyro_interface& gyro)
{
    return gyro.yaw_rate;
}


static inline float gyro_interface_get_yaw_rate_dps(gyro_interface& gyro)
{
    return gyro.yaw_rate_dps;
}

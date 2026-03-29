#include <zf_common_headfile.hpp>
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include "zf_device_imu.hpp"

int main()
{
    zf_device_imu imu;

    imu_device_type_enum type = imu.init();

    if (type == DEV_NO_FIND)
    {
        std::printf("IMU init failed\r\n");
        return -1;
    }

    std::printf("IMU init success, type = %d\r\n", (int)type);

    while (1)
    {
        int16 gz = imu.get_gyro_z();

        std::printf("gyro_z_raw = %d\r\n", gz);

        usleep(20000);
    }

    return 0;
}

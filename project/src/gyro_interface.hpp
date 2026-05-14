#pragma once

#include <cmath>
#include <unistd.h>

#include "zf_common_typedef.hpp"
#include "zf_device_imu.hpp"

#include "control_params.hpp"

class GyroInterface {
public:
  GyroInterface()
  {
    auto &p = g_params.gyro;

    imu_type = imu.init();

    raw_z = 0;
    bias_z = 0.0f;
    raw_z_corr = 0.0f;
    yaw_rate_dps = 0.0f;
    yaw_rate = 0.0f;
    bias_ready = false;

    if (imu_type == DEV_NO_FIND) {
      valid = false;
      return;
    }

    set_scale(p.gyro_scale);
    valid = true;
  }

  void set_scale(float gyro_scale)
  {
    if (gyro_scale > 0.0f) {
      this->gyro_scale = gyro_scale;
    }
  }

  int16 read_raw_z()
  {
    if (! valid) {
      return 0;
    }

    return imu.get_gyro_z();
  }

  uint8 calibrate_bias(int sample_count, int sample_delay_ms)
  {
    long sum = 0;

    if (! valid) {
      return 0;
    }

    if (sample_count <= 0) {
      return 0;
    }

    for (int i = 0; i < sample_count; i++) {
      sum += read_raw_z();

      if (sample_delay_ms > 0) {
        usleep(sample_delay_ms * 1000);
      }
    }

    bias_z = static_cast<float>(sum) / static_cast<float>(sample_count);
    bias_ready = true;

    return 1;
  }

  void update()
  {
    if (! valid) {
      raw_z = 0;
      raw_z_corr = 0.0f;
      yaw_rate_dps = 0.0f;
      yaw_rate = 0.0f;
      return;
    }

    raw_z = read_raw_z();
    raw_z_corr = static_cast<float>(raw_z) - bias_z;

    if (gyro_scale > 0.0f) {
      yaw_rate_dps = raw_z_corr / gyro_scale;
    }
    else {
      yaw_rate_dps = 0.0f;
    }

    yaw_rate = yaw_rate_dps * 3.1415926f / 180.0f;
  }

  void clear()
  {
    raw_z = 0;
    raw_z_corr = 0.0f;
    yaw_rate_dps = 0.0f;
    yaw_rate = 0.0f;
  }

  float get_yaw_rate() const
  {
    return yaw_rate;
  }

  float get_yaw_rate_dps() const
  {
    return yaw_rate_dps;
  }

  bool is_valid() const
  {
    return valid;
  }

private:
  zf_device_imu imu{};
  imu_device_type_enum imu_type = DEV_NO_FIND;

  int16 raw_z = 0;
  float bias_z = 0.0f;
  float raw_z_corr = 0.0f;

  float yaw_rate_dps = 0.0f;
  float yaw_rate = 0.0f;

  float gyro_scale = 16.4f;

  bool valid = false;
  bool bias_ready = false;
};

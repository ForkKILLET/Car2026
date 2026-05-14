#pragma once
#include <algorithm>
#include <cmath>

#include "zf_common_typedef.hpp"
#include "zf_driver_gpio.hpp"
#include "zf_driver_pwm.hpp"

class Motor {
public:
  Motor(const char *pwm_device, const char *dir_device) : pwm_(pwm_device), dir_(dir_device, O_RDWR)
  {
  }

  pwm_info pwm_info_{};
  zf_driver_pwm pwm_;
  zf_driver_gpio dir_;

  void init();
  void deinit();

  void set_duty(int16 duty);
  void set_duty_cycle(float duty_cycle);
  int16 get_duty();
  float get_duty_cycle();
};

void Motor::init()
{
  pwm_.get_dev_info(&pwm_info_);
}

void Motor::deinit()
{
  pwm_.set_duty(0);
}

void Motor::set_duty(int16 duty)
{
  dir_.set_level(duty >= 0);
  pwm_.set_duty(std::abs(duty));
}

void Motor::set_duty_cycle(float duty_cycle)
{
  duty_cycle = std::clamp(duty_cycle, -1.0f, +1.0f);
  int16 duty = static_cast<int16>(pwm_info_.duty_max * duty_cycle);
  set_duty(duty);
}

int16 Motor::get_duty()
{
  pwm_.get_dev_info(&pwm_info_);
  uint16 duty = static_cast<uint16>(pwm_info_.duty);
  return dir_.get_level() ? +duty : -duty;
}

float Motor::get_duty_cycle()
{
  int16 duty = get_duty();
  return static_cast<float>(duty) / static_cast<float>(pwm_info_.duty_max);
}

#pragma once
#include "zf_driver_pwm.hpp"
#include "zf_driver_gpio.hpp"
#include "zf_common_typedef.hpp"

#include <algorithm>
#include <cmath>

class fp_motor {
public:
  fp_motor(const char* pwm_device, const char* dir_device, bool dir_inverted) :
    pwm_(pwm_device),
    dir_(dir_device, O_RDWR),
    dir_inverted_(dir_inverted)
  {}

  pwm_info pwm_info_{};
  zf_driver_pwm pwm_;
  zf_driver_gpio dir_;
  bool dir_inverted_;

  void init();
  void deinit();

  void set_duty(int16 duty);
  void set_duty_cycle(float duty_cycle);
  int16 get_duty();
  float get_duty_cycle();
};

void fp_motor::init() {
  pwm_.get_dev_info(&pwm_info_);
}

void fp_motor::deinit() {
  pwm_.set_duty(0);
}

void fp_motor::set_duty(int16 duty) {
  uint8 original_level = duty >= 0;
  dir_.set_level(original_level != dir_inverted_);
  pwm_.set_duty(std::abs(duty));
}

void fp_motor::set_duty_cycle(float duty_cycle) {
  duty_cycle = std::clamp(duty_cycle, -1.0f, +1.0f);
  int16 duty = static_cast<int16>(pwm_info_.duty_max * duty_cycle);
  set_duty(duty);
}

int16 fp_motor::get_duty() {
  pwm_.get_dev_info(&pwm_info_);
  uint16 duty = static_cast<uint16>(pwm_info_.duty);
  uint8 original_level = dir_.get_level();
  return original_level != dir_inverted_ ? + duty : - duty;
}

float fp_motor::get_duty_cycle() {
  int16 duty = get_duty();
  return static_cast<float>(duty) / static_cast<float>(pwm_info_.duty_max);
}

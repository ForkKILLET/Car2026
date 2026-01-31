#pragma once
#include <zf_common_headfile.hpp>

class fp_motor {
public:
  fp_motor(const char* pwm_device, const char* dir_device) :
    pwm_(pwm_device),
    dir_(dir_device, O_RDWR)
  {}

  pwm_info pwm_info_ {};
  zf_driver_pwm pwm_;
  zf_driver_gpio dir_;

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
  dir_.set_level(duty >= 0);
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
  return dir_.get_level() ? + duty : - duty;
}

float fp_motor::get_duty_cycle() {
  int16 duty = get_duty();
  return static_cast<float>(duty) / static_cast<float>(pwm_info_.duty_max);
}

#pragma once
#include <zf_common_headfile.hpp>
#include "task.hpp"
#include "motor.hpp"

constexpr uint32 TIMER_INTERVAL = 10;
constexpr useconds_t DUTY_LOOP_INTERVAL = 50;
constexpr uint16 DUTY_LOOP_PERIOD = 10000;
constexpr float DUTY_CYCLE_MAX = 0.5f;
constexpr uint8 DUTY_LOOP_TIMES = DUTY_LOOP_PERIOD / DUTY_LOOP_INTERVAL;
constexpr float DUTY_CYCLE_STEP = (2.0f * DUTY_CYCLE_MAX) / DUTY_LOOP_TIMES;

class fp_motor_test : public fp_task {
public:
  void init() override;
  void deinit() override;
  void work() override;

private:
  zf_driver_pit timer{};
  fp_motor motor_left = fp_motor(ZF_PWM_MOTOR_2, ZF_GPIO_MOTOR_2);
  fp_motor motor_right = fp_motor(ZF_PWM_MOTOR_1, ZF_GPIO_MOTOR_1);

  float duty_cycle = 0.0f;
  float duty_cycle_delta = DUTY_CYCLE_STEP;
  uint8 duty_t = DUTY_LOOP_TIMES / 2;
};

void fp_motor_test::init() {
  motor_left.init();
  motor_right.init();
  timer.init_ms(TIMER_INTERVAL, [] {});
}

void fp_motor_test::deinit() {
  timer.stop();
  motor_left.deinit();
  motor_right.deinit();
}

void fp_motor_test::work() {
  while (true) {
    motor_left.set_duty_cycle(duty_cycle);
    motor_right.set_duty_cycle(duty_cycle);

    std::cout
      << "Left: " << motor_left.get_duty_cycle() << ", "
      << "Right: " << motor_right.get_duty_cycle() << ", "
      << "Target:" << duty_cycle << std::endl;

    duty_t ++;
    duty_cycle += duty_cycle_delta;
    if (duty_t == DUTY_LOOP_TIMES) {
      duty_t = 0;
      duty_cycle_delta = - duty_cycle_delta;
    }

    system_delay_ms(DUTY_LOOP_INTERVAL);
  }
}

#pragma once

#include "control_params.hpp"
#include "encoder_system.hpp"
#include "motor.hpp"
#include "motor_speed_control.hpp"

class ChassisControl {
public:
  ChassisControl(const char *motor_l_pwm,
                 const char *motor_l_gpio,
                 const char *motor_r_pwm,
                 const char *motor_r_gpio) :
      motor_l(motor_l_pwm, motor_l_gpio), motor_r(motor_r_pwm, motor_r_gpio)
  {
  }

  // 初始化
  void init(const ControlParams &params)
  {
    encoder_sys.init(params.encoder);

    // 电机初始化
    motor_l.init();
    motor_r.init();

    motor_l_ctrl.init(params.motor_l_pid);

    motor_r_ctrl.init(params.motor_r_pid);

    // 初始目标速度设为 0
    motor_l_ctrl.set_target(0.0f);
    motor_r_ctrl.set_target(0.0f);
  }

  // 设置左右轮目标速度
  void set_target_speed(float target_l, float target_r)
  {
    motor_l_ctrl.set_target(target_l);
    motor_r_ctrl.set_target(target_r);
  }

  // 获取左右轮当前速度
  float get_speed_l() const
  {
    return encoder_sys.get_speed_l();
  }

  float get_speed_r() const
  {
    return encoder_sys.get_speed_r();
  }

  // 获取车体线速度与角速度
  float get_speed_car() const
  {
    return encoder_sys.get_speed_car();
  }

  float get_yaw_rate() const
  {
    return encoder_sys.get_yaw_rate();
  }

  // 闭环更新
  void loop()
  {
    // 左轮
    motor_l_ctrl.set_current(encoder_sys.get_speed_l());
    motor_l_ctrl.update(motor_l);

    // 右轮
    motor_r_ctrl.set_current(encoder_sys.get_speed_r());
    motor_r_ctrl.update(motor_r);
  }

  // 停车
  void stop()
  {
    motor_l_ctrl.stop(motor_l);
    motor_r_ctrl.stop(motor_r);
  }

  // 清零
  void clear()
  {
    encoder_sys.clear();

    motor_l_ctrl.stop(motor_l);
    motor_r_ctrl.stop(motor_r);
  }

  int16 get_pwm_l() const
  {
    return motor_l_ctrl.get_pwm_out();
  }

  int16 get_pwm_r() const
  {
    return motor_r_ctrl.get_pwm_out();
  }

private:
  FpMotor motor_l;
  FpMotor motor_r;

  MotorSpeedControl motor_l_ctrl{};
  MotorSpeedControl motor_r_ctrl{};
};

static ChassisControl chassis{
    "/dev/zf_pwm_motor_2", "/dev/zf_gpio_motor_2", "/dev/zf_pwm_motor_1", "/dev/zf_gpio_motor_1"};

#pragma once
#include "zf_common_typedef.hpp"

#include "chassis_control.hpp"
#include "control_params.hpp"

// 差速解算
// 基本公式：
// target_l = base_speed - turn
// target_r = base_speed + turn
// turn = w * wheel_base / 2

class DiffDrive {
public:
  // 初始化
  DiffDrive() {}

  // 设置输入
  void set_input(float base_speed, float turn)
  {
    this->base_speed = base_speed;
    this->turn = turn;
  }

  // 差速解算
  void update()
  {
    // 差速解算
    target_l = base_speed - turn;
    target_r = base_speed + turn;
  }

  // 左右轮闭环
  void apply()
  {
    chassis.set_target_speed(target_l, target_r);
  }

  void loop(float base_speed, float turn)
  {
    set_input(base_speed, turn);
    update();
    apply();
  }

  float get_target_l() const
  {
    return target_l;
  }

  float get_target_r() const
  {
    return target_r;
  }

private:
  // 输入
  float base_speed = 0.0f; // 基础速度（m/s）
  float turn = 0.0f;       // 转向修正量

  // 输出
  float target_l = 0.0f; // 左轮目标速度
  float target_r = 0.0f; // 右轮目标速度
};

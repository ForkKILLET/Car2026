#pragma once
#include "zf_common_typedef.hpp"
#include "chassis_control.hpp"

// 差速解算
// 基本公式：
// target_l = base_speed - turn
// target_r = base_speed + turn
// turn = w * wheel_base / 2

struct diff_drive
{
  // 输入
  float base_speed = 0.0f;    // 基础速度（m/s）
  float turn = 0.0f;          // 转向修正量

  // 输出
  float target_l = 0.0f;      // 左轮目标速度
  float target_r = 0.0f;      // 右轮目标速度
};

// 初始化
static inline void diff_drive_init(diff_drive &drive)
{
  drive.base_speed = 0.0f;
  drive.turn = 0.0f;

  drive.target_l = 0.0f;
  drive.target_r = 0.0f;
}

// 设置输入
static inline void diff_drive_set_input(diff_drive &drive,
                                        float base_speed,
                                        float turn)
{
  drive.base_speed = base_speed;
  drive.turn = turn;
}

// 差速解算
static inline void diff_drive_update(diff_drive &drive)
{
  // 差速解算
  drive.target_l = drive.base_speed - drive.turn;
  drive.target_r = drive.base_speed + drive.turn;
}

// 左右轮闭环
static inline void diff_drive_apply(diff_drive &drive)
{
  chassis_set_target_speed(drive.target_l, drive.target_r);
}

static inline void diff_drive_loop(diff_drive &drive,
                                  float base_speed,
                                  float turn)
{
  diff_drive_set_input(drive, base_speed, turn);
  diff_drive_update(drive);
  diff_drive_apply(drive);
}

#pragma once
#include "zf_driver_encoder.hpp"
#include "zf_driver_pit.hpp"
#include "zf_common_typedef.hpp"

struct encoder_speed_system
{
  // 左右编码器对象
  zf_driver_encoder encoder_l;
  zf_driver_encoder encoder_r;

  // 定时器对象
  zf_driver_pit timer;

  // 本周期左右编码器计数
  int16 count_l;
  int16 count_r;

  // 软件累计总数
  int32 total_l;
  int32 total_r;

  // 左右轮原始速度
  float speed_l_raw;
  float speed_r_raw;

  // 滤波后速度
  float speed_l;
  float speed_r;

  // 小车线速度
  float speed_car;

  // 小车角速度
  float yaw_rate;

  // 左右轮每个编码器计数对应的路程
  float meter_per_count_l;
  float meter_per_count_r;

  // 左右轮中心距
  float wheel_base;

  // 一阶低通滤波系数
  float alpha;

  // 定时器周期
  float dt_s;
  int timer_ms;

  // 构造函数
  encoder_speed_system(const char *encoder_l_path,
                       const char *encoder_r_path)
      : encoder_l(encoder_l_path, O_RDWR),
        encoder_r(encoder_r_path, O_RDWR),
        timer{}
  {
    count_l = 0;
    count_r = 0;

    total_l = 0;
    total_r = 0;

    speed_l_raw = 0.0f;
    speed_r_raw = 0.0f;

    speed_l = 0.0f;
    speed_r = 0.0f;

    speed_car = 0.0f;
    yaw_rate = 0.0f;

    meter_per_count_l = 0.0f;
    meter_per_count_r = 0.0f;

    wheel_base = 0.0f;
    alpha = 0.5f;

    dt_s = 0.01f;
    timer_ms = 10;
  }
};

static encoder_speed_system encoder_sys("/dev/zf_encoder_quad_1",
                                                   "/dev/zf_encoder_quad_2");

// pit 定时器回调函数
static inline void encoder_speed_timer_callback(void)
{
  encoder_speed_system &sys = encoder_sys;

  // 读取本周期编码器计数
  sys.count_l = sys.encoder_l.get_count();
  sys.count_r = sys.encoder_r.get_count();

  // 立刻清零
  sys.encoder_l.clear_count();
  sys.encoder_r.clear_count();

  // 累计总数
  sys.total_l += sys.count_l;
  sys.total_r += sys.count_r;

  // 原始速度
  sys.speed_l_raw = (float)(sys.count_l) * sys.meter_per_count_l / sys.dt_s;

  sys.speed_r_raw = (float)(sys.count_r) * sys.meter_per_count_r / sys.dt_s;

  // 一阶低通滤波
  sys.speed_l = sys.alpha * sys.speed_l_raw + (1.0f - sys.alpha) * sys.speed_l;

  sys.speed_r = sys.alpha * sys.speed_r_raw + (1.0f - sys.alpha) * sys.speed_r;

  // 小车线速度
  sys.speed_car = (sys.speed_l + sys.speed_r) * 0.5f;

  // 小车角速度
  if (sys.wheel_base > 0.000001f)
  {
    sys.yaw_rate = (sys.speed_r - sys.speed_l) / sys.wheel_base;
  }
  else
  {
    sys.yaw_rate = 0.0f;
  }
}

// 初始化
static inline void encoder_speed_init(float meter_per_count_l,
                                      float meter_per_count_r,
                                      float wheel_base,
                                      float alpha,
                                      int timer_ms)
{
  encoder_speed_system &sys = encoder_sys;

  sys.count_l = 0;
  sys.count_r = 0;

  sys.total_l = 0;
  sys.total_r = 0;

  sys.speed_l_raw = 0.0f;
  sys.speed_r_raw = 0.0f;

  sys.speed_l = 0.0f;
  sys.speed_r = 0.0f;

  sys.speed_car = 0.0f;
  sys.yaw_rate = 0.0f;

  sys.meter_per_count_l = meter_per_count_l;
  sys.meter_per_count_r = meter_per_count_r;

  sys.wheel_base = wheel_base;
  sys.alpha = alpha;

  sys.timer_ms = timer_ms;
  sys.dt_s = (float) timer_ms / 1000.0f;

  // 先清零编码器
  sys.encoder_l.clear_count();
  sys.encoder_r.clear_count();

  // 启动定时器
  sys.timer.init_ms(timer_ms, encoder_speed_timer_callback);
}

// 停止
static inline void encoder_speed_stop(encoder_speed_system &sys)
{
  sys.timer.stop();
}

// 清零
static inline void encoder_speed_clear(encoder_speed_system &sys)
{
  sys.count_l = 0;
  sys.count_r = 0;

  sys.total_l = 0;
  sys.total_r = 0;

  sys.speed_l_raw = 0.0f;
  sys.speed_r_raw = 0.0f;

  sys.speed_l = 0.0f;
  sys.speed_r = 0.0f;

  sys.speed_car = 0.0f;
  sys.yaw_rate = 0.0f;

  sys.encoder_l.clear_count();
  sys.encoder_r.clear_count();
}

#pragma once
#include <zf_common_headfile.hpp>

#include "control_params.hpp"
#include "maf_filter.hpp"

class EncoderSys {
public:
  EncoderSys()
  {
    auto &params = g_params.encoder;
    meter_per_count_l = params.meter_per_count_l;
    meter_per_count_r = params.meter_per_count_r;
    wheel_base = params.wheel_base;
    alpha = params.alpha;
    dt_s = static_cast<float>(g_params.timer.encoder_period_ms) / 1000.0f;

    // 清零编码器
    encoder_l.clear_count();
    encoder_r.clear_count();

    setup_timer();
  }

  void clear()
  {
    count_l = 0;
    count_r = 0;

    // speed_l_filter.clear();
    // speed_r_filter.clear();

    speed_l = 0.0f;
    speed_r = 0.0f;

    speed_car = 0.0f;
    yaw_rate = 0.0f;

    encoder_l.clear_count();
    encoder_r.clear_count();
  }

  void loop()
  {
    // 读取本周期编码器计数
    count_l = encoder_l.get_count();
    count_r = encoder_r.get_count();

    // 立刻清零
    encoder_l.clear_count();
    encoder_r.clear_count();

    // 原始速度
    float speed_l_raw = static_cast<float>(count_l) * meter_per_count_l / dt_s;
    float speed_r_raw = static_cast<float>(count_r) * meter_per_count_r / dt_s;

    // 速度滤波
    // speed_l = speed_l_filter.sample(speed_l_raw);
    // speed_r = speed_r_filter.sample(speed_r_raw);
    speed_l = alpha * speed_l_raw + (1.0f - alpha) * speed_l;
    speed_r = alpha * speed_r_raw + (1.0f - alpha) * speed_r;

    // 小车线速度
    speed_car = (speed_l + speed_r) * 0.5f;

    // 小车角速度
    yaw_rate = 0.0f;
  }

  float get_speed_l() const
  {
    return speed_l;
  }

  float get_speed_r() const
  {
    return speed_r;
  }

  float get_speed_car() const
  {
    return speed_car;
  }

  float get_yaw_rate() const
  {
    return yaw_rate;
  }

  static EncoderSys &instance()
  {
    static EncoderSys encoder_sys{};
    return encoder_sys;
  }

  static void timer_callback()
  {
    instance().loop();
  }

  static void setup_timer()
  {
    static zf_driver_pit timer{};
    timer.init_ms(g_params.timer.encoder_period_ms, timer_callback);
  }

private:
  // 左右编码器对象
  zf_driver_encoder encoder_l{"/dev/zf_encoder_quad_2"};
  zf_driver_encoder encoder_r{"/dev/zf_encoder_quad_1"};

  // 本周期左右编码器计数
  int16 count_l = 0;
  int16 count_r = 0;

  // 速度滤波器
  // MafFilter<5> speed_l_filter{};
  // MafFilter<5> speed_r_filter{};

  float alpha = 0.1f;

  // 滤波后速度
  float speed_l = 0.0f;
  float speed_r = 0.0f;

  // 小车线速度
  float speed_car = 0.0f;

  // 小车角速度
  float yaw_rate = 0.0f;

  // 左右轮每个编码器计数对应的路程
  float meter_per_count_l = 0.0f;
  float meter_per_count_r = 0.0f;

  // 左右轮中心距
  float wheel_base = 0.0f;

  // 定时器周期
  float dt_s = 0.01f;
};

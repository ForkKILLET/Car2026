#pragma once
#include "control_params.hpp"
#include "zf_driver_encoder.hpp"
#include "zf_driver_pit.hpp"
#include "zf_common_typedef.hpp"

static inline void encoder_speed_timer_callback(void);

class EncoderSystem {
public:
  EncoderSystem(const char *encoder_l_path, const char *encoder_r_path) :
      encoder_l(encoder_l_path, O_RDWR), encoder_r(encoder_r_path, O_RDWR)
  {
  }

  void init(const EncoderParams &params)
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

    meter_per_count_l = params.meter_per_count_l;
    meter_per_count_r = params.meter_per_count_r;

    wheel_base = params.wheel_base;
    alpha = params.alpha;

    timer_ms = params.timer_ms;
    dt_s = static_cast<float>(params.timer_ms) / 1000.0f;

    // 先清零编码器
    encoder_l.clear_count();
    encoder_r.clear_count();

    // 启动定时器
    timer.init_ms(params.timer_ms, encoder_speed_timer_callback);
  }

  void stop()
  {
    timer.stop();
  }

  void clear()
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

    encoder_l.clear_count();
    encoder_r.clear_count();
  }

  void on_timer()
  {
    // 读取本周期编码器计数
    count_l = encoder_l.get_count();
    count_r = encoder_r.get_count();

    // 立刻清零
    encoder_l.clear_count();
    encoder_r.clear_count();

    // 累计总数
    total_l += count_l;
    total_r += count_r;

    // 原始速度
    speed_l_raw = static_cast<float>(count_l) * meter_per_count_l / dt_s;

    speed_r_raw = static_cast<float>(count_r) * meter_per_count_r / dt_s;

    // 一阶低通滤波
    speed_l = alpha * speed_l_raw + (1.0f - alpha) * speed_l;

    speed_r = alpha * speed_r_raw + (1.0f - alpha) * speed_r;

    // 小车线速度
    speed_car = (speed_l + speed_r) * 0.5f;

    // 小车角速度
    if (wheel_base > 0.000001f) {
      yaw_rate = (speed_r - speed_l) / wheel_base;
    }
    else {
      yaw_rate = 0.0f;
    }
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

private:
  // 左右编码器对象
  zf_driver_encoder encoder_l;
  zf_driver_encoder encoder_r;

  // 定时器对象
  zf_driver_pit timer{};

  // 本周期左右编码器计数
  int16 count_l = 0;
  int16 count_r = 0;

  // 软件累计总数
  int32 total_l = 0;
  int32 total_r = 0;

  // 左右轮原始速度
  float speed_l_raw = 0.0f;
  float speed_r_raw = 0.0f;

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

  // 一阶低通滤波系数
  float alpha = 0.5f;

  // 定时器周期
  float dt_s = 0.01f;
  int timer_ms = 10;
};

static EncoderSystem encoder_sys("/dev/zf_encoder_quad_1", "/dev/zf_encoder_quad_2");

// pit 定时器回调函数
static inline void encoder_speed_timer_callback(void)
{
  encoder_sys.on_timer();
}

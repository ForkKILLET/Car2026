#pragma once
#include <zf_common_headfile.hpp>
#include "task.hpp"
#include "pid.hpp"

static pid_ctrl pid_l{};
static pid_ctrl pid_r{};
static auto encoder_l = zf_driver_encoder("/dev/zf_encoder_quad_2");
static auto encoder_r = zf_driver_encoder("/dev/zf_encoder_quad_1");

static constexpr float encoder_count_ratio = 9494.6;
static constexpr float encoder_prop = encoder_count_ratio / 0.02;

static constexpr float target_speed = 1.0;

bool encoder_initialized = false;
int16 l_count = 0;
int16 r_count = 0;

class fp_pid_test : public fp_task {
public:

  void work() override {
    // 初始化 PID
    pid_set(pid_l, PID_INC, 0.8, 0.2, 0.05);
    pid_limit(pid_l, 0xFFFF, 80.0);
    pid_set(pid_r, PID_INC, 0.8, 0.2, 0.05);
    pid_limit(pid_r, 0xFFFF, 80.0);

    // 初始化定时器
    zf_driver_pit timer{};
    timer.init_ms(20, on_timer);
  }

  static void on_timer() {
    int16 l_count_now = encoder_l.get_count();
    int16 r_count_now = encoder_r.get_count();

    int16 l_count_delta = l_count_now - l_count;
    int16 r_count_delta = r_count_now - r_count;

    if (! encoder_initialized) {
      encoder_initialized = true;
      return;
    }

    // d = 64 mm
    // C = 0.201 m
    // Δn_C = 1907
    // k = C / Δn_C = 9494.6 m^-1
    // v = Δx / Δt = Δn * encoder_count_ratio / Δt_timer = Δn * encoder_prop
    float l_current_speed = l_count_delta * encoder_prop;
    float r_current_speed = r_count_delta * encoder_prop;

    pid_increment(pid_l, target_speed, l_current_speed);
    pid_increment(pid_r, target_speed, r_current_speed);
  }
};

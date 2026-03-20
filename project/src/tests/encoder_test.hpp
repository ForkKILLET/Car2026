#pragma once
#include <zf_common_headfile.hpp>

class fp_encoder_test : public fp_task {
public:
  void work() override {
    auto encoder_l = zf_driver_encoder("/dev/zf_encoder_quad_2");
    auto encoder_r = zf_driver_encoder("/dev/zf_encoder_quad_1");

    while (true) {

      std::cout
        << "encoder left: " << encoder_l.get_count() << ", "
        << "right: " << encoder_r.get_count() << std::endl;

      system_delay_ms(100);
    }
  }
};

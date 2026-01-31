#pragma once
#include <zf_common_headfile.hpp>

class encoder_test : public fp_task {
public:
  void work() override {
    while (true) {
      auto encoder_1 = zf_driver_encoder("/dev/zf_encoder_1");
      auto encoder_2 = zf_driver_encoder("/dev/zf_encoder_2");

      std::cout
        << "encoder_1: " << encoder_1.get_count() << ", "
        << "encoder_2: " << encoder_2.get_count() << std::endl;

      system_delay_ms(100);
    }
  }
};

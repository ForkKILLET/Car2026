#include <zf_common_headfile.hpp>

#include "task.hpp"

constexpr useconds_t FPS = 1;
constexpr useconds_t MSPF = 1000 / FPS;

class screen_test : public fp_task {
public:
  void init() override;
  void work() override;
  void paint();

private:
  zf_device_ips200 ips200_{};
};

void screen_test::init() {
  ips200_.init(FB_PATH);
}

void screen_test::work() {
  std::cout << "Start testing screen ..." << std::endl;
  while (true) {
    ips200_.clear();
    paint();
    system_delay_ms(MSPF);
  }
}

void screen_test::paint() {
  std::cout << "Painting ..." << std::endl;
  ips200_.full(RGB565_39C5BB);
}

#include <zf_common_headfile.hpp>

#include "tests/motor_duty_loop_test.hpp"

int main(int argc, char** argv) {
  motor_duty_loop_test test{};
  test.run();
  return 0;
}

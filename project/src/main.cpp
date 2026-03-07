#include <zf_common_headfile.hpp>

#include "tests/motor_duty_loop_test.hpp"
#include "tests/encoder_test.hpp"

int main(int argc, char* argv[]) {
  encoder_test test{};
  test.run();
  return 0;
}

#include <zf_common_headfile.hpp>
#include <iostream>

#include "tests/motor_test.hpp"
#include "tests/encoder_test.hpp"

int main(int argc, char* argv[]) {
  fp_motor_test test{};
  test.run();
  return 0;
}

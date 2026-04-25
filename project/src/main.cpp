#include "debug_test.hpp"

static debug_test dbg;

int main(int argc, char* argv[]) {
  fp_encoder_test test{};
  test.run();
  return 0;
}

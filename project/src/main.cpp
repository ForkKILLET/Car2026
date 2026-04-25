#include "zf_common_headfile.hpp"
#include "debug_test.hpp"

int main(void)
{
  debug_test_init();

  // 停车
  // debug_test_set_mode(debug_mode_stop);

  // 左轮单独闭环测试
  // debug_test_set_mode(debug_mode_left_speed);
  // debug_test_set_target_speed(10.0f, 0.0f);

  // 右轮单独闭环测试
  // debug_test_set_mode(debug_mode_right_speed);
  // debug_test_set_target_speed(10.0f, 0.25f);

  // 双轮同速直行测试
  // debug_test_set_mode(debug_mode_dual_speed);
  // debug_test_set_target_speed(5.0f, 5.0f);

  // 差速转弯测试
  debug_test_set_mode(debug_mode_diff_test);
  debug_test_set_diff(3.0f, 0.5f);

  // 伪偏差循迹测试
  // debug_test_set_mode(debug_mode_line_fake);
  // debug_test_set_line(10.0f, 1);

  debug_test_start();

  while (1)
  {
  }

  return 0;
}

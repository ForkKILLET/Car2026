#include "debug_test.hpp"

static debug_test dbg;

int main(void)
{
    debug_test_init(dbg);

    // 停车
    debug_test_set_mode(dbg, debug_mode_stop);

    // 左轮单独闭环测试
    // debug_test_set_mode(dbg, debug_mode_left_speed);
    // debug_test_set_target_speed(dbg, 0.25f, 0.0f);

    // 右轮单独闭环测试
    // debug_test_set_mode(dbg, debug_mode_right_speed);
    // debug_test_set_target_speed(dbg, 0.0f, 0.25f);

    // 双轮同速直行测试
    // debug_test_set_mode(dbg, debug_mode_dual_speed);
    // debug_test_set_target_speed(dbg, 0.25f, 0.25f);

    // 差速转弯测试
    // debug_test_set_mode(dbg, debug_mode_diff_test);
    // debug_test_set_diff(dbg, 0.25f, 0.05f);

    // 伪偏差循迹测试
    // debug_test_set_mode(dbg, debug_mode_line_fake);
    // debug_test_set_line(dbg, 10.0f, 1);

    while (1)
    {
    }

    return 0;
}

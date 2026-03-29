#pragma once

#include "zf_common_typedef.hpp"
#include "control_manager.hpp"

// 状态
typedef enum
{
    state_stop = 0,     // 停车
    state_ready,        // 准备运行
    state_run,          // 正常运行
    state_lost_line,    // 丢线
    state_error         // 异常
} control_state_id;

struct state_manager
{
    // 当前状态
    control_state_id state;

    // 上一个状态
    control_state_id last_state;

    // 启动命令
    uint8 start_cmd;

    // 异常标志
    uint8 error_flag;

    // 丢线计数
    uint32 lost_line_count;

    // 丢线允许持续的最大周期数
    uint32 lost_line_limit;

    state_manager()
    {
        state = state_stop;
        last_state = state_stop;

        start_cmd = 0;
        error_flag = 0;

        lost_line_count = 0;
        lost_line_limit = 10;   // 默认允许丢线 10 个控制周期
    }
};

// 初始化
static inline void state_manager_init(state_manager& sm)
{
    sm.state = state_stop;
    sm.last_state = state_stop;

    sm.start_cmd = 0;
    sm.error_flag = 0;

    sm.lost_line_count = 0;
    sm.lost_line_limit = 10;
}




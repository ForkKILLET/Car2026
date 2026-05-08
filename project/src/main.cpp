#include "zf_common_headfile.hpp"

#include "debug/camera_report.hpp"
#include "debug/debugger_conn.hpp"

int main(void)
{
  auto client = conn_debugger();

  zf_device_uvc uvc{};
  if (uvc.init("/dev/video0") < 0) {
    fp_err("Failed to initialize UVC device\n");
    return 1;
  }
  fp_log("UVC device initialized\n");

  CameraReport camera_report{uvc, client};

  camera_report.start();

  return 0;
}

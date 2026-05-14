#include "zf_common_headfile.hpp"

#include "control_manager.hpp"
#include "debug/camera_report.hpp"
#include "debug/control_test.hpp"
#include "debug/data_report.hpp"
#include "debug/debugger_client.hpp"

int main(void)
{
  /********** Report **********/
  // zf_device_uvc uvc{};
  // if (uvc.init("/dev/video0") < 0) {
  //   fp_err("Failed to initialize UVC device\n");
  //   return 1;
  // }
  // fp_log("UVC device initialized\n");

  // CameraReport camera_report{uvc};

  // camera_report.start();

  /********** Main **********/
  zf_device_uvc uvc{};
  if (uvc.init("/dev/video0") < 0) {
    fp_err("Failed to initialize UVC device\n");
    return 1;
  }

  auto &ctrl = CtrlManager::instance();
  ctrl.start();
  CtrlManager::setup_timer();

  Vision vision{};

  while (true) {
    if (uvc.wait_image_refresh() < 0) {
      fp_err("Failed to refresh camera frame\n");
      continue;
    }

    auto image = reinterpret_cast<Image>(uvc.get_gray_image_ptr());
    vision.process_image(image);

    const auto &result = vision.result();
    ctrl.set_line(result.offset);
  }

  /********** Dual Speed Test **********/
  // ControlTest &test = ControlTest::instance();
  // test.set_mode(DebugMode::DualSpeed);
  // test.set_target_speed(2.5f, 2.5f);
  // ControlTest::setup_timer();

  // while (true) {}

  return 0;
}

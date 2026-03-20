#include <zf_common_headfile.hpp>
#include "utils.hpp"
#include "client.hpp"

constexpr size_t FRAME_SIZE = UVC_WIDTH * UVC_HEIGHT * 2;

class fp_camera_report {
public:
  fp_camera_report(zf_device_uvc uvc, fp_client& client) : uvc_(uvc), client_(client) {}

  void report_frame();

private:
  zf_device_uvc& uvc_;
  fp_client& client_;
};

void fp_camera_report::report_frame() {
  if (uvc_.wait_image_refresh() < 0) {
    fp_log("Failed to refresh camera frame\n");
    return;
  }

  uint16* image = uvc_.get_rgb_image_ptr();

  client_.send(reinterpret_cast<uint8*>(image), FRAME_SIZE);
}

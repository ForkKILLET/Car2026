#include <zf_common_headfile.hpp>

#include "client.hpp"
#include "utils.hpp"

constexpr size_t FRAME_SIZE = UVC_WIDTH * UVC_HEIGHT * 2;

class CameraReport {
public:
  CameraReport(zf_device_uvc &uvc, Client &client) : uvc_(uvc), client_(client) {}

  void report_frame();
  void start();

private:
  zf_device_uvc &uvc_;
  Client &client_;

  uint16 seq_ = 0;
};

void CameraReport::report_frame()
{
  if (uvc_.wait_image_refresh() < 0) {
    fp_err("Failed to refresh camera frame\n");
    return;
  }

  uint16 *image = uvc_.get_rgb_image_ptr();

  fp_log("Reporting frame %u\n", seq_++);
  client_.send(reinterpret_cast<uint8 *>(image), FRAME_SIZE);
}

void CameraReport::start()
{
  while (true) {
    report_frame();
  }

  fp_log("Camera report stopped\n");
}

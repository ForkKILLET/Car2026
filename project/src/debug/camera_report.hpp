#include <zf_common_headfile.hpp>

#include "client.hpp"
#include "utils.hpp"

constexpr size_t FRAME_SIZE = UVC_WIDTH * UVC_HEIGHT * 2;

enum class CameraReportMode {
  Color,
  Gray,
  Vision,
};

class CameraReport {
public:
  CameraReport() = delete;

  CameraReport(zf_device_uvc &uvc) : uvc_(uvc) {}

  void set_mode(CameraReportMode mode);
  void report_frame();
  void start();

private:
  zf_device_uvc &uvc_;
  Client &client_{get_debugger_client(2027)};
  CameraReportMode mode_{CameraReportMode::Color};
  Vision vision_{};

  uint16 seq_ = 0;
};

void CameraReport::report_frame()
{
  if (uvc_.wait_image_refresh() < 0) {
    fp_err("Failed to refresh camera frame\n");
    return;
  }
  // Always send RGB565 frames over TCP so backend + frontend rendering code stays unchanged.
  // For GRAY and VISION modes we convert/annotate into RGB565 before sending.
  uint16 out_frame[UVC_WIDTH * UVC_HEIGHT];
  const size_t out_bytes = FRAME_SIZE;

  if (mode_ == CameraReportMode::Color) {
    // device already produces rgb565
    uint16 *image_ptr = uvc_.get_rgb_image_ptr();
    std::memcpy(out_frame, image_ptr, out_bytes);
  }
  else if (mode_ == CameraReportMode::Gray) {
    // convert gray -> rgb565 gray
    uint8 *gray = uvc_.get_gray_image_ptr();
    for (size_t i = 0; i < (size_t) UVC_WIDTH * UVC_HEIGHT; ++i) {
      uint8 g = gray[i];
      // expand to rgb565
      uint16 r = (g * 31) / 255;
      uint16 gg = (g * 63) / 255;
      uint16 b = (g * 31) / 255;
      uint16 val = (uint16) ((r << 11) | (gg << 5) | b);
      out_frame[i] = val;
    }
  }
  else { // VISION
    uint8 *gray = uvc_.get_gray_image_ptr();
    vision_.process_image(reinterpret_cast<Image>(gray));
    ConstImage label_img = vision_.image();
    for (size_t y = 0; y < UVC_HEIGHT; ++y) {
      for (size_t x = 0; x < UVC_WIDTH; ++x) {
        uint8 t = label_img[y][x];
        uint16 val;
        switch (t) {
        case MID_LINE:
          val = RGB565_YELLOW;
          break;
        case BOUND:
          val = RGB565_GREEN;
          break;
        case BOUND_APP:
          val = RGB565_CYAN;
          break;
        case SPECIAL:
          val = RGB565_MAGENTA;
          break;
        case ROAD:
          val = RGB565_WHITE;
          break;
        default:
          val = RGB565_BLACK;
          break;
        }
        out_frame[y * UVC_WIDTH + x] = val;
      }
    }
  }

  fp_log("Reporting frame %u\n", seq_++);
  client_.send(reinterpret_cast<const uint8 *>(out_frame), out_bytes);
}

void CameraReport::start()
{
  // Small buffer for incoming control messages from server
  std::string ctrl_buf;
  uint8 read_buf[1024];

  while (true) {
    report_frame();

    // poll for control messages (non-blocking)
    int n = client_.recv(read_buf, sizeof(read_buf));
    if (n > 0) {
      ctrl_buf.append(reinterpret_cast<char *>(read_buf), n);
      // try to extract newline-terminated JSON messages
      size_t pos;
      while ((pos = ctrl_buf.find('\n')) != std::string::npos) {
        std::string line = ctrl_buf.substr(0, pos);
        ctrl_buf.erase(0, pos + 1);
        // look for "mode":"..."
        auto mpos = line.find("\"mode\"");
        if (mpos != std::string::npos) {
          auto col = line.find(':', mpos);
          if (col != std::string::npos) {
            auto dq1 = line.find('"', col);
            if (dq1 != std::string::npos) {
              auto dq2 = line.find('"', dq1 + 1);
              if (dq2 != std::string::npos && dq2 > dq1) {
                std::string mode_s = line.substr(dq1 + 1, dq2 - dq1 - 1);
                if (mode_s == "color")
                  set_mode(CameraReportMode::Color);
                else if (mode_s == "gray")
                  set_mode(CameraReportMode::Gray);
                else if (mode_s == "vision")
                  set_mode(CameraReportMode::Vision);
              }
            }
          }
        }
      }
    }
  }

  fp_log("Camera report stopped\n");
}

// helper to change mode safely
void CameraReport::set_mode(CameraReportMode mode)
{
  mode_ = mode;
  // reset sequence/frame buffer if needed
  seq_ = 0;
}

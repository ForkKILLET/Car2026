#include <asio.hpp>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <exception>
#include <iostream>
#include <json/json.h>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "vision.hpp"

static volatile std::sig_atomic_t stop_requested = 0;

using asio::ip::tcp;

// Read exactly n bytes from socket
static bool read_exact(tcp::socket &socket, std::vector<uint8_t> &buf, size_t n)
{
  buf.resize(n);
  asio::error_code ec;
  size_t received = asio::read(socket, asio::buffer(buf.data(), n), asio::transfer_exactly(n), ec);
  if (ec)
    return false;
  if (received != n) {
    buf.resize(received);
    return false;
  }
  return true;
}

// Read one line of input (no newline in result)
static bool read_line(tcp::socket &socket, std::string &line)
{
  line.clear();
  char ch = '\0';
  while (true) {
    asio::error_code ec;
    size_t n = asio::read(socket, asio::buffer(&ch, 1), asio::transfer_exactly(1), ec);
    if (ec || n != 1)
      return false;
    if (ch == '\n')
      break;
    line += ch;
  }
  return true;
}

// Send JSON line (with newline)
static bool send_json(tcp::socket &socket, const Json::Value &json)
{
  Json::StreamWriterBuilder w;
  w["indentation"] = "";
  std::string s = Json::writeString(w, json);
  s += '\n';

  asio::error_code ec;
  asio::write(socket, asio::buffer(s), ec);
  return ! ec;
}

// Send binary data
static bool send_data(tcp::socket &socket, const uint8_t *data, size_t size)
{
  asio::error_code ec;
  asio::write(socket, asio::buffer(data, size), ec);
  return ! ec;
}

static bool handle_connection(tcp::socket &socket, Vision &vision, int width, int height)
{
  while (! stop_requested) {
    std::string line;
    Json::Value frameStart;

    if (! read_line(socket, line))
      return false;

    try {
      Json::CharReaderBuilder reader;
      std::string errors;
      std::istringstream iss(line);
      if (! Json::parseFromStream(reader, iss, &frameStart, &errors))
        return false;
    }
    catch (...) {
      return false;
    }

    if (frameStart.get("type", "").asString() != "frameStart")
      return false;

    int dataSize = frameStart.get("dataSize", -1).asInt();
    if (dataSize != width * height)
      return false;

    std::vector<uint8_t> frame_buf;
    if (! read_exact(socket, frame_buf, (size_t) dataSize))
      return false;

    Json::Value frameEnd;
    if (! read_line(socket, line))
      return false;

    try {
      Json::CharReaderBuilder reader;
      std::string errors;
      std::istringstream iss(line);
      if (! Json::parseFromStream(reader, iss, &frameEnd, &errors))
        return false;
    }
    catch (...) {
      return false;
    }

    if (frameEnd.get("type", "").asString() != "frameEnd")
      return false;

    Image img = reinterpret_cast<Image>(frame_buf.data());
    vision.process_image(img);

    const auto &result = vision.result();
    const auto &image = vision.image();

    Json::Value out;
    out["type"] = "result";
    out["visionOffset"] = result.offset;
    out["visionElement"] = static_cast<int>(result.element_type);
    if (! send_json(socket, out))
      return false;

    Json::Value imageStart;
    imageStart["type"] = "annotatedFrameStart";
    imageStart["width"] = width;
    imageStart["height"] = height;
    imageStart["pixelFormat"] = "label8";
    imageStart["dataSize"] = static_cast<Json::Value::Int>(width * height);
    if (! send_json(socket, imageStart))
      return false;

    const uint8_t *img_ptr = reinterpret_cast<const uint8_t *>(image);
    if (! send_data(socket, img_ptr, width * height))
      return false;

    Json::Value imageEnd;
    imageEnd["type"] = "annotatedFrameEnd";
    if (! send_json(socket, imageEnd))
      return false;
  }

  return true;
}

int main()
{
  const char *port_env = std::getenv("VISION_DEBUG_PORT");
  int port = port_env ? std::atoi(port_env) : 2029;

  std::cerr << "vision_debug: listening on port " << port << std::endl;

  Vision vision;
  asio::io_context io_context;

  tcp::acceptor acceptor(io_context);
  std::mutex socket_mutex;
  std::shared_ptr<tcp::socket> current_socket;
  asio::error_code ec;
  acceptor.open(tcp::v4(), ec);
  if (ec) {
    std::cerr << "acceptor.open failed: " << ec.message() << std::endl;
    return 1;
  }

  acceptor.set_option(asio::socket_base::reuse_address(true), ec);
  if (ec) {
    std::cerr << "acceptor.set_option failed: " << ec.message() << std::endl;
    return 1;
  }

  acceptor.bind(tcp::endpoint(tcp::v4(), static_cast<unsigned short>(port)), ec);
  if (ec) {
    std::cerr << "acceptor.bind failed: " << ec.message() << std::endl;
    return 1;
  }

  acceptor.listen(asio::socket_base::max_listen_connections, ec);
  if (ec) {
    std::cerr << "acceptor.listen failed: " << ec.message() << std::endl;
    return 1;
  }

  asio::signal_set signals(io_context, SIGINT);
  signals.async_wait([&](const asio::error_code &signal_ec, int) {
    if (signal_ec)
      return;
    stop_requested = 1;
    std::cout << "\nInterrupted by user" << std::endl;

    asio::error_code ignored_ec;
    acceptor.cancel(ignored_ec);
    acceptor.close(ignored_ec);

    std::lock_guard<std::mutex> lock(socket_mutex);
    if (current_socket) {
      current_socket->cancel(ignored_ec);
      current_socket->close(ignored_ec);
    }
  });
  std::thread signal_thread([&]() { io_context.run(); });

  while (! stop_requested) {
    auto socket = std::make_shared<tcp::socket>(io_context);
    {
      std::lock_guard<std::mutex> lock(socket_mutex);
      current_socket = socket;
    }

    acceptor.accept(*socket, ec);
    if (ec) {
      if (stop_requested || ec == asio::error::operation_aborted)
        break;
      std::cerr << "accept failed: " << ec.message() << std::endl;
      continue;
    }
    handle_connection(*socket, vision, WIDTH, HEIGHT);

    {
      std::lock_guard<std::mutex> lock(socket_mutex);
      if (current_socket == socket)
        current_socket.reset();
    }
  }

  io_context.stop();
  signal_thread.join();

  return 0;
}

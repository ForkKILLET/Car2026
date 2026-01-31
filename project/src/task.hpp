#pragma once
#include <zf_common_headfile.hpp>

class fp_task {
public:
  virtual void init() {}
  virtual void deinit() {}
  virtual void work() {}

  bool initialized_{false};

  ~fp_task() {
    if (initialized_) deinit();
  }

  void run() {
    if (initialized_) throw std::runtime_error("Task already initialized");
    initialized_ = true;

    init();
    work();
  }
};

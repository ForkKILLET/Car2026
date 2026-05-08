#pragma once
#include <zf_common_headfile.hpp>

class Task {
public:
  virtual void init()
  {
  }
  virtual void deinit()
  {
  }
  virtual void work()
  {
  }

  bool initialized_{false};

  ~Task()
  {
    if (initialized_)
      deinit();
  }

  void run()
  {
    if (initialized_)
      throw std::runtime_error("Task already initialized");
    initialized_ = true;

    init();
    work();
  }
};

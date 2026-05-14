#pragma once
#include <array>
#include <cstddef>

template <size_t window_size> class MafFilter {
public:
  static_assert(window_size > 0, "window_size must be greater than 0");

  float sample(float value)
  {
    if (count == window_size) {
      sum -= window[index];
    }
    else {
      count++;
    }

    window[index] = value;
    sum += value;
    avg = sum / static_cast<float>(count);

    if (++index == window_size)
      index = 0;

    return avg;
  }

  float get() const
  {
    return avg;
  }

  void clear()
  {
    index = 0;
    count = 0;
    sum = 0.0f;
    avg = 0.0f;
  }

private:
  std::array<float, window_size> window{};
  size_t index = 0;
  size_t count = 0;
  float sum = 0.0f;
  float avg = 0.0f;
};

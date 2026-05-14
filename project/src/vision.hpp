#pragma once

#include <cmath>
#include <cstdlib>
#include <cstring>

#include "utils.hpp"

constexpr uint8_t HEIGHT = 120;
constexpr uint8_t WIDTH = 160;
constexpr uint8_t X_MID = WIDTH / 2;
constexpr uint8_t X_MIN = 1;
constexpr uint8_t X_MAX = WIDTH - 1;
constexpr uint8_t Y_MAX = HEIGHT - 1;

constexpr uint8_t OSTU_COUNTER_MAX = 10;
constexpr uint8_t OTSU_THRESHOLD_MIN = 20;
constexpr uint8_t OTSU_THRESHOLD_MAX = 200;
constexpr uint8_t OTSU_COMPRESS_RATIO = 1;
constexpr uint8_t OTSU_COMPRESS_RATIO_SQUARE = 1;
constexpr uint8_t OTSU_THRESHOLD_DELTA = 4;

constexpr uint8_t Y_SEARCH_MIN = 35;
constexpr uint8_t Y_NORMAL_MIN = 60;
constexpr uint8_t Y_BD_MIN = 38;
constexpr uint8_t Y_BOTTOM_MIN = 78;
constexpr uint8_t Y_BOTTOM_BOTH_LOST_MIN = 110;
constexpr uint8_t Y_LP_END_MIN = 35;
constexpr uint8_t Y_XR_TOP_MIN = 50;
constexpr uint8_t Y_XR_TOP_MAX = 80;
constexpr uint8_t X_XR_TOP_MIN = WIDTH * 0.25;
constexpr uint8_t X_XR_TOP_MAX = WIDTH * 0.75;
constexpr uint8_t Y_XR_M_HEIGHT = 5;
constexpr uint8_t Y_XR_M_OFFSET = 2;
constexpr uint8_t Y_STRICT_G_MAX = 45;
constexpr uint8_t Y_MID_LINE_MIN = 95;
constexpr uint8_t X_CURVE_OFFSET = 25;
constexpr uint8_t DX_BD_MAX = 5;
constexpr uint8_t DX_BD_INV_MAX = 5;
constexpr uint8_t DX_M_MAX = 2;
constexpr uint8_t BD_LENGTH = 85;
constexpr uint8_t BD_NG_COUNT_MAX = 3;
constexpr uint8_t BD_G_COUNT_MAX = 2;
constexpr int8_t CV_CONVEX = +4;
constexpr int8_t LP_CONVEX = -4;
constexpr uint8_t LP_UP_COUNT_MAX = 3;
constexpr uint8_t LP_UP_FAILED_MAX = 3;
constexpr uint8_t LE_DN_MAX = 3;
constexpr uint8_t LE_UP_MAX = 3;
constexpr uint8_t LE_UP_COUNT_MAX = 3;
constexpr uint8_t Y_LP_MIN = 30;
constexpr uint8_t X_LP_CORNER_R_MIN = X_MIN;
constexpr uint8_t X_LP_CORNER_R_MAX = WIDTH * 0.8;
constexpr uint8_t X_LP_CORNER_L_MIN = WIDTH * 0.2;
constexpr uint8_t X_LP_CORNER_L_MAX = X_MAX;
constexpr uint8_t X_LP_CORNER_L_OFFSET = 5;
constexpr uint8_t X_LP_CORNER_R_OFFSET = 10;
constexpr uint8_t CONVEX_LIMIT = 2;
constexpr uint8_t BOTH_COUNT_MIN = 5;

constexpr uint8_t EMPTY = 0;
constexpr uint8_t ROAD = 1;
constexpr uint8_t BOUND = 2;
constexpr uint8_t MID_LINE = 3;
constexpr uint8_t BOUND_APP = 4;
constexpr uint8_t SPECIAL = 5;

using Image = uint8_t (*)[WIDTH];
using ConstImage = const uint8_t (*)[WIDTH];
using ImagePtr = uint8_t *;

enum ElementType {
  Zebra,
  CurveLeft,
  CurveRight,
  Normal,
  CrossBefore,
  Cross,
  LoopLeftBefore,
  LoopRightBefore,
  LoopLeftBefore2,
  LoopRightBefore2,
  LoopLeft,
  LoopRight,
  LoopLeftAfter,
  LoopRightAfter,
  RampLeft,
  RampRight,
};

struct ImageResult {
  double offset;
  ElementType element_type;
};

static constexpr uint8_t STD_WIDTH[HEIGHT] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,
    3,  4,  5,  6,  7,  8,  8,  9,  10, 11, 12, 13, 14, 15, 16, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 25, 26, 27, 28, 29, 30, 31, 32, 33, 33, 34, 35, 36, 37, 38, 39, 40, 41, 41, 42, 43, 44, 45,
    46, 47, 48, 49, 50, 50, 51, 52, 53, 54, 55, 56, 57, 58, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67};

class Vision {
public:
  Vision() = default;

  void process_image(Image image);

  const ImageResult &result() const
  {
    return image_result_;
  }

  ConstImage image() const
  {
    return image_;
  }

private:
  enum class BoundStatus { None, Good, Bad };
  enum class SearchState { Bottom, NormalBound, Loop, CrossBound, Analyze, Mid, Done };

  void set_point(Image image, uint8_t x, uint8_t y, uint8_t type);
  uint8_t otsu_calc_threshold(Image image, uint8_t min, uint8_t max);
  void otsu_binarize_image(Image image, uint8_t threshold);
  void search(Image image);

  ImageResult image_result_{};
  uint8_t otsu_counter_ = 0;
  uint8_t otsu_threshold_ = 0;
  uint8_t image_[HEIGHT][WIDTH] = {0};
  bool loop_enabled_ = true;
};

void Vision::set_point(Image image, uint8_t x, uint8_t y, uint8_t type)
{
#ifdef IMAGE_DEBUG
  image[y][x] = type;
#endif
}

void Vision::process_image(Image image)
{
  if (image) {
    std::memcpy(image_, image, sizeof(image_));
  }

  if (otsu_counter_) {
    otsu_counter_--;
  }
  else {
    if (otsu_threshold_)
      otsu_threshold_ = otsu_calc_threshold(
          image_, otsu_threshold_ - OTSU_THRESHOLD_DELTA, otsu_threshold_ + OTSU_THRESHOLD_DELTA);
    else
      otsu_threshold_ = otsu_calc_threshold(image_, OTSU_THRESHOLD_MIN, OTSU_THRESHOLD_MAX);
    otsu_counter_ = OSTU_COUNTER_MAX;
  }
  otsu_binarize_image(image_, otsu_threshold_);
  search(image_);
}

uint8_t Vision::otsu_calc_threshold(Image image, uint8_t min, uint8_t max)
{
  const int OTSU_PIXEL_COUNT = HEIGHT * WIDTH / OTSU_COMPRESS_RATIO_SQUARE;
  double var_max = 0;
  uint8_t k_best = min;
  for (uint8_t k = min; k <= max; k++) {
    int c = 0, s = 0, sm = 0;
    for (int i = 0; i < HEIGHT; i += OTSU_COMPRESS_RATIO) {
      for (int j = 0; j < WIDTH; j += OTSU_COMPRESS_RATIO) {
        if (image[i][j] <= k) {
          c++;
          sm += image[i][j];
        }
        s += image[i][j];
      }
    }
    double p1 = (double) c / OTSU_PIXEL_COUNT;
    if (p1 == 0)
      continue;
    if (p1 == 1)
      break;

    double mg = (double) s / OTSU_PIXEL_COUNT;
    double m = (double) sm / OTSU_PIXEL_COUNT;
    double var = pow(mg * p1 - m, 2) / (p1 * (1 - p1));
    if (var > var_max) {
      var_max = var;
      k_best = k;
    }
  }
  return k_best;
}

void Vision::otsu_binarize_image(Image image, uint8_t threshold)
{
  for (uint8_t i = 0; i < HEIGHT; i++)
    for (uint8_t j = 0; j < WIDTH; j++)
      image[i][j] = image[i][j] > threshold;
}

void Vision::search(Image image)
{
  ElementType el = image_result_.element_type;
  bool prev_el_normal = el <= Normal;

  // Zebra detection
  if (image[Y_MAX][0] != ROAD) {
    uint8_t curr = ROAD;
    uint8_t change_count = 0;
    for (uint8_t x = X_MIN; x <= X_MAX; x++) {
      if (image[Y_MAX][x] != curr) {
        curr = image[Y_MAX][x];
        change_count++;
      }
      if (change_count > 9) {
        image_result_.element_type = Zebra;
        return;
      }
    }
    if (el == Zebra)
      image_result_.element_type = Normal;
  }

  uint8_t xl = 0, xr = 0, y = 0;
  int16_t dx = 0, xm = X_MID;
  uint8_t xls[HEIGHT] = {0}, xrs[HEIGHT] = {0};
  double ml = 0;
  double mr = 0;
  bool ml_set = false, mr_set = false;
  bool l_pad = false, r_pad = false;

  uint8_t y0l = 0, y0r = 0;
  uint8_t y_start = 0, y_end = 0;
  bool l_unset = true, r_unset = true;
  bool lost = false;
  int8_t l_convex = 0, r_convex = 0;
  uint8_t l_l = 0, l_r = 0, r_l = 0, r_r = 0;
  uint8_t l_l_cont = 0, r_r_cont = 0;
  int8_t l_dx = 0, r_dx = 0;
  bool le_failed = false;

  uint8_t l_ng_count = 0, r_ng_count = 0;
  uint8_t l_g_count = BD_G_COUNT_MAX, r_g_count = BD_G_COUNT_MAX;
  bool l_ng = true, r_ng = true;
  bool l_stop0 = false, r_stop0 = false, l_stop = false, r_stop = false;
  uint8_t l_segs = 0, r_segs = 0;
  uint16_t sw = 0;
  double so = 0;
  double kw = 1;

  SearchState state = SearchState::Bottom;
  while (state != SearchState::Done) {
    switch (state) {
    case SearchState::Bottom: {
      for (y = Y_MAX; y > Y_BOTTOM_BOTH_LOST_MIN; y--) {
        xl = xr = X_MID;
        if (image[y][X_MID] != EMPTY) {
          while (image[y][xl - 1] != EMPTY)
            if (--xl == X_MIN)
              break;
          while (image[y][xr + 1] != EMPTY)
            if (++xr == X_MAX)
              break;
        }
        else {
          while (true) {
            if (--xl == X_MIN)
              break;
            if (image[y][xl] != EMPTY) {
              xr = xl;
              while (image[y][xl - 1] != EMPTY)
                if (--xl == X_MIN)
                  break;
              break;
            }
            xr++;
            if (image[y][xr + 1] != EMPTY) {
              xl = xr;
              while (image[y][xr + 1] != EMPTY)
                if (++xr == X_MAX)
                  break;
              break;
            }
          }
        }
        if (xl != X_MIN || xr != X_MAX) {
          if (xl != X_MIN)
            xls[y] = xl;
          if (xr != X_MAX)
            xrs[y] = xr;
          break;
        }
      }
      l_pad = xls[Y_MAX] == X_MIN;
      r_pad = xrs[Y_MAX] == X_MAX;

      y_end = y;
      set_point(image, xl, y, BOUND);
      set_point(image, xr, y, BOUND);

      if (xl == X_MIN)
        for (y = y_end; image[y--][X_MIN] != EMPTY;)
          if (y == Y_BOTTOM_MIN) {
            l_stop0 = l_stop = l_ng = true;
            break;
          }

      if (xr == X_MAX)
        for (y = y_end; image[y--][X_MAX] != EMPTY;)
          if (y == Y_BOTTOM_MIN) {
            r_stop0 = true;
            r_ng = true;
            break;
          }

      if (l_stop0 && r_stop0) {
        return;
      }

      if (el == LoopRight || el == LoopLeft) {
        state = SearchState::Loop;
        break;
      }
      if (el == Cross) {
        state = SearchState::CrossBound;
        break;
      }
      state = SearchState::NormalBound;
      break;
    }
    case SearchState::NormalBound: {
      y0l = y0r = 0;
      l_unset = r_unset = true;
      ml_set = mr_set = false;
      l_convex = r_convex = 0;
      l_l = l_r = r_l = r_r = 0;
      l_segs = r_segs = 0;
      y_end = Y_MAX;
      le_failed = false;

      for (y = Y_MAX - 1, dx = 0; y >= Y_BD_MIN; y--) {
        if (y == Y_NORMAL_MIN && prev_el_normal && ! l_ng_count && ! r_ng_count && l_segs == 1 &&
            r_segs == 1) {
          break;
        }

        BoundStatus left_status = BoundStatus::None;
        if (! l_stop) {
          lost = false;
          for (dx = 0; image[y][xl] == EMPTY; dx++, xl++)
            if (dx == DX_BD_MAX) {
              xl -= DX_BD_MAX;
              lost = true;
              break;
            }
          if (! dx && xl != X_MIN) {
            for (; image[y][xl - 1] != EMPTY && xl > X_MIN; dx--, xl--) {
              if (dx == -DX_BD_INV_MAX + l_l_cont) {
                xl += DX_BD_INV_MAX;
                if (! le_failed && image[y - 1][xl] == EMPTY) {
                  uint8_t xc = xl, yc = y;
                  uint8_t dy = 0;
                  uint8_t up_count = 0;
                  bool exit_lost = false;
                  for (; xc <= X_MAX; xc++) {
                    for (dy = 0; image[yc][xc] == EMPTY; yc++)
                      if (++dy == LE_DN_MAX) {
                        exit_lost = true;
                        break;
                      }
                    if (exit_lost)
                      break;
                    if (image[yc - 1][xc] != EMPTY) {
                      if (++up_count == LE_UP_COUNT_MAX) {
                        exit_lost = true;
                        break;
                      }
                      for (dy = 1, yc--; image[yc - 1][xc] != EMPTY; yc--)
                        if (++dy == LE_UP_MAX) {
                          exit_lost = true;
                          break;
                        }
                      if (exit_lost)
                        break;
                    }
                    set_point(image, xc, yc, BOUND);
                  }
                  if (! exit_lost && xc > X_MAX) {
                    y_start = yc;
                    double xlf = xls[Y_MAX];
                    set_point(image, xls[Y_MAX], Y_MAX, SPECIAL);
                    double m = (double) (xc - xlf) / (Y_MAX - yc);
                    for (y = Y_MAX - 1; y > yc; y--) {
                      xlf += m;
                      xls[y] = (uint8_t) xlf;
                      set_point(image, (uint8_t) xlf, y, BOUND_APP);
                    }
                    state = SearchState::Mid;
                    break;
                  }
                  if (exit_lost)
                    le_failed = true;
                }
                lost = true;
                break;
              }
            }
          }
          if (state == SearchState::Mid)
            break;

          if (xl == X_MIN) {
            l_unset = true;
            left_status = BoundStatus::Bad;
          }
          else if (l_unset) {
            y0l = y;
            l_unset = false;
          }

          if (lost) {
            if (el > Cross)
              l_stop = true;
            else if (ml_set) {
              xl = ml * (y - y0l) + xls[y0l];
              xls[y] = xl;
              set_point(image, xl, y, BOUND_APP);
              left_status = BoundStatus::Bad;
            }
          }
          else if (xl != X_MIN) {
            if (! l_unset && y - y0l && dx > 0 && dx <= DX_M_MAX && xl - xrs[y0l]) {
              ml = (double) (xl - xls[y0l]) / (y - y0l);
              ml_set = true;
            }
            xls[y] = xl;
            set_point(image, xl, y, BOUND);

            dx = xl - xls[y + 1];
            if (dx > 0) {
              l_r++;
              l_l_cont = 0;
            }
            else {
              l_l++;
              l_l_cont++;
            }
            l_convex += limit(dx - l_dx, CONVEX_LIMIT);
            if (l_convex >= CV_CONVEX && prev_el_normal) {
              el = image_result_.element_type = CurveRight;
              break;
            }
            else if (l_convex <= LP_CONVEX && el == LoopLeftBefore) {
              el = image_result_.element_type = LoopLeftBefore2;
              prev_el_normal = false;
            }
            l_dx = dx;

            if (l_ng_count) {
              l_ng_count = 0;
              if (y > Y_STRICT_G_MAX)
                l_g_count = BD_G_COUNT_MAX;
              else
                left_status = BoundStatus::Good;
            }
            else if (l_g_count) {
              if (--l_g_count == 0)
                left_status = BoundStatus::Good;
            }
          }
        }

        if (left_status == BoundStatus::Good) {
          if (l_ng) {
            l_ng = false;
            l_segs++;
          }
        }
        else if (left_status == BoundStatus::Bad) {
          l_g_count = 0;
          if (++l_ng_count == BD_NG_COUNT_MAX)
            l_ng = true;
        }

        if (state == SearchState::Mid)
          break;

        BoundStatus right_status = BoundStatus::None;
        if (! r_stop) {
          lost = false;
          for (dx = 0; image[y][xr] == EMPTY; dx--, xr--)
            if (dx == -DX_BD_MAX) {
              xr += DX_BD_MAX;
              lost = true;
              break;
            }
          if (! dx && xr != X_MAX) {
            for (; image[y][xr + 1] != EMPTY && xr < X_MAX; dx++, xr++) {
              if (dx == DX_BD_INV_MAX - r_r_cont) {
                xr -= DX_BD_INV_MAX;
                if (! le_failed && l_stop0 && image[y - 1][xr] == EMPTY) {
                  uint8_t xc = xr, yc = y;
                  uint8_t dy = 0;
                  uint8_t up_count = 0;
                  bool exit_lost = false;
                  for (; xc >= X_MIN; xc--) {
                    for (dy = 0; image[yc][xc] == EMPTY; yc++)
                      if (++dy == LE_DN_MAX) {
                        exit_lost = true;
                        break;
                      }
                    if (exit_lost)
                      break;
                    if (image[yc - 1][xc] != EMPTY) {
                      if (++up_count == LE_UP_COUNT_MAX) {
                        exit_lost = true;
                        break;
                      }
                      for (dy = 1, yc--; image[yc - 1][xc] != EMPTY; yc--)
                        if (++dy == LE_UP_MAX) {
                          exit_lost = true;
                          break;
                        }
                      if (exit_lost)
                        break;
                    }
                    set_point(image, xc, yc, BOUND);
                  }
                  if (! exit_lost && xc < X_MIN) {
                    y_start = yc;
                    double xrf = xrs[Y_MAX];
                    set_point(image, xrs[Y_MAX], Y_MAX, SPECIAL);
                    double m = (double) (xc - xrf) / (Y_MAX - yc);
                    for (y = Y_MAX - 1; y > yc; y--) {
                      xrf += m;
                      xrs[y] = (uint8_t) xrf;
                      set_point(image, (uint8_t) xrf, y, BOUND_APP);
                    }
                    state = SearchState::Mid;
                    break;
                  }
                  if (exit_lost)
                    le_failed = true;
                }
                lost = true;
                break;
              }
            }
          }
          if (state == SearchState::Mid)
            break;

          if (xr == X_MAX) {
            r_unset = true;
            right_status = BoundStatus::Bad;
          }
          else if (r_unset) {
            y0r = y;
            r_unset = false;
          }

          if (lost) {
            if (el > Cross)
              r_stop = true;
            else if (mr_set) {
              xr = mr * (y - y0r) + xrs[y0r];
              xrs[y] = xr;
              set_point(image, xr, y, BOUND_APP);
              right_status = BoundStatus::Bad;
            }
          }
          else if (xr != X_MAX) {
            if (! r_unset && y - y0r && dx < 0 && dx >= -DX_M_MAX && xr != xrs[y0r]) {
              mr = (double) (xr - xrs[y0r]) / (y - y0r);
              mr_set = true;
            }
            xrs[y] = xr;
            set_point(image, xr, y, BOUND);

            dx = -xr + xrs[y + 1];
            if (dx > 0) {
              r_l++;
              r_r_cont = 0;
            }
            else {
              r_r++;
              r_r_cont++;
            }
            r_convex += limit(dx - r_dx, CONVEX_LIMIT);
            if (r_convex >= CV_CONVEX && prev_el_normal) {
              el = image_result_.element_type = CurveLeft;
              break;
            }
            else if (r_convex <= LP_CONVEX && el == LoopRightBefore) {
              el = image_result_.element_type = LoopRightBefore2;
              prev_el_normal = false;
            }
            r_dx = dx;

            if (r_ng_count) {
              r_ng_count = 0;
              if (y > Y_STRICT_G_MAX)
                r_g_count = BD_G_COUNT_MAX;
              else
                right_status = BoundStatus::Good;
            }
            else if (r_g_count) {
              if (--r_g_count == 0)
                right_status = BoundStatus::Good;
            }
          }
        }

        if (right_status == BoundStatus::Good) {
          if (r_ng) {
            r_ng = false;
            r_segs++;
          }
        }
        else if (right_status == BoundStatus::Bad) {
          r_g_count = 0;
          if (++r_ng_count == BD_NG_COUNT_MAX)
            r_ng = true;
        }

        if (state == SearchState::Mid)
          break;
      }
      if (state == SearchState::Mid)
        break;
      y_start = y;
      state = SearchState::Analyze;
      break;
    }
    case SearchState::Loop: {
      y_end = Y_MAX;
      if (el == LoopLeft) {
        for (y = Y_MAX; image[y][X_MIN] == EMPTY && y > Y_LP_MIN; y--)
          ;
        for (; image[y - 1][X_MIN] != EMPTY; y--)
          if (y == Y_LP_MIN) {
            el = image_result_.element_type = Normal;
            state = SearchState::NormalBound;
            break;
          }
        if (state == SearchState::NormalBound)
          break;
        set_point(image, X_MIN, y, SPECIAL);

        uint8_t y0 = 0, y1 = 0, xc = 0, yc = 0;
        uint8_t up_count = 0, up_failed_count = 0;

        bool loop_cancel = false;
        for (xl = X_MIN + 1;; xl++) {
          if (xl == X_LP_CORNER_L_MAX) {
            loop_cancel = true;
            break;
          }
          if (image[y - 1][xl] != EMPTY) {
            if (++up_count == LP_UP_COUNT_MAX) {
              if (xl < X_LP_CORNER_L_MIN) {
                loop_cancel = true;
                break;
              }
              xc = std::max((uint8_t) (xl - LP_UP_COUNT_MAX - X_LP_CORNER_L_OFFSET), X_MIN);
              y_start = yc = y = y0;
              set_point(image, xc, yc, SPECIAL);
              break;
            }
            for (y1 = y, y--; image[y - 1][xl] != EMPTY; y--)
              if (y == Y_LP_MIN) {
                if (++up_failed_count == LP_UP_FAILED_MAX) {
                  loop_cancel = true;
                  break;
                }
                y = y1;
                break;
              }
            if (loop_cancel)
              break;
          }
          else {
            up_count = 0;
            y0 = y;
          }
          for (y1 = y; image[y][xl] == EMPTY; y++)
            if (y == y1 - LP_UP_COUNT_MAX) {
              y = y1;
              break;
            }
          set_point(image, xl, y, BOUND);
        }

        if (loop_cancel) {
          el = image_result_.element_type = Normal;
          state = SearchState::NormalBound;
          break;
        }

        double xrf = xrs[Y_MAX];
        set_point(image, xrs[Y_MAX], Y_MAX, SPECIAL);
        double m = (double) (xc - xrf) / (Y_MAX - yc);
        for (y = Y_MAX - 1; y > yc; y--) {
          xrf += m;
          xrs[y] = (uint8_t) xrf;
          set_point(image, (uint8_t) xrf, y, BOUND_APP);
        }
      }
      else {
        for (y = Y_MAX; image[y][X_MAX] == EMPTY && y > Y_LP_MIN; y--)
          ;
        for (; image[y - 1][X_MAX] != EMPTY; y--)
          if (y == Y_LP_MIN) {
            el = image_result_.element_type = Normal;
            state = SearchState::NormalBound;
            break;
          }
        if (state == SearchState::NormalBound)
          break;
        set_point(image, X_MAX, y, BOUND);

        uint8_t y0 = 0, xc = 0, yc = 0;
        uint8_t up_count = 0, up_failed_count = 0;

        bool loop_cancel = false;
        for (xr = X_MAX - 1;; xr--) {
          if (xr == X_LP_CORNER_R_MIN) {
            loop_cancel = true;
            break;
          }
          if (image[y - 1][xr] != EMPTY) {
            if (++up_count == LP_UP_COUNT_MAX) {
              if (xr > X_LP_CORNER_R_MAX) {
                loop_cancel = true;
                break;
              }
              xc = std::min((uint8_t) (xr + X_LP_CORNER_R_OFFSET), X_MAX);
              y_start = yc = y;
              set_point(image, xc, yc, SPECIAL);
              break;
            }
            for (y0 = y, y--; image[y - 1][xr] != EMPTY; y--)
              if (y == Y_LP_MIN) {
                if (++up_failed_count == LP_UP_FAILED_MAX) {
                  loop_cancel = true;
                  break;
                }
                y = y0;
                break;
              }
            if (loop_cancel)
              break;
          }
          else {
            up_count = 0;
          }
          while (image[y][xr] == EMPTY && y < Y_MAX)
            y++;
          set_point(image, xr, y, BOUND);
        }

        if (loop_cancel) {
          el = image_result_.element_type = Normal;
          state = SearchState::NormalBound;
          break;
        }

        double xlf = xls[Y_MAX];
        set_point(image, xls[Y_MAX], Y_MAX, SPECIAL);
        double m = (double) (xc - xlf) / (Y_MAX - yc);
        for (y = Y_MAX - 1; y > yc; y--) {
          xlf += m;
          xls[y] = (uint8_t) xlf;
          set_point(image, (uint8_t) xlf, y, BOUND_APP);
        }
      }
      state = SearchState::Mid;
      break;
    }
    case SearchState::CrossBound: {
      y = Y_XR_TOP_MIN;
      bool cross_found = false;
      while (true) {
        bool cross_next = false;
        xl = xr = X_MID;
        if (image[y][X_MID] != EMPTY) {
          while (image[y][xl - 1] != EMPTY)
            if (--xl < X_XR_TOP_MIN) {
              cross_next = true;
              break;
            }
          if (! cross_next) {
            while (image[y][xr + 1] != EMPTY)
              if (++xr > X_XR_TOP_MAX) {
                cross_next = true;
                break;
              }
          }
        }
        else {
          while (true) {
            if (--xl < X_XR_TOP_MIN) {
              cross_next = true;
              break;
            }
            if (image[y][xl] != EMPTY) {
              xr = xl;
              while (image[y][xl - 1] != EMPTY)
                if (--xl < X_XR_TOP_MIN) {
                  cross_next = true;
                  break;
                }
              break;
            }
            xr++;
            if (image[y][xr + 1] != EMPTY) {
              xl = xr;
              while (image[y][xr + 1] != EMPTY)
                if (++xr > X_XR_TOP_MAX) {
                  cross_next = true;
                  break;
                }
              break;
            }
          }
        }
        if (! cross_next) {
          dx = xr - xl;
          if (dx > 5 && dx <= STD_WIDTH[y] * 3) {
            cross_found = true;
            break;
          }
        }
        y += 3;
        if (y > Y_XR_TOP_MAX)
          break;
      }
      if (! cross_found) {
        el = image_result_.element_type = Normal;
        state = SearchState::NormalBound;
        break;
      }

      y_start = y;
      set_point(image, xl, y, MID_LINE);
      set_point(image, xr, y, MID_LINE);
      bool cross_top = true;
      for (y++; y < Y_MAX; y++) {
        for (dx = 0; image[y][xl - 1] != EMPTY; dx++, xl--)
          if (dx == DX_BD_MAX) {
            cross_top = false;
            break;
          }
        if (! dx)
          for (; image[y][xl] == EMPTY; xl++)
            ;
        xls[y] = xl;
        set_point(image, xl, y, BOUND);
        for (dx = 0; image[y][xr + 1] != EMPTY; dx++, xr++)
          if (dx == DX_BD_MAX) {
            cross_top = false;
            break;
          }
        if (! dx)
          for (; image[y][xr] == EMPTY; xr--)
            ;
        xrs[y] = xr;
        if (! cross_top)
          break;
        set_point(image, xr, y, BOUND);
      }
      y_end = y;
      if (y == Y_MAX) {
        el = image_result_.element_type = Normal;
        state = SearchState::NormalBound;
        break;
      }

      uint8_t y0 = std::max(y_start, (uint8_t) (y - Y_XR_M_HEIGHT - Y_XR_M_OFFSET));
      uint8_t y1 = y - Y_XR_M_OFFSET;
      ml = (double) (xls[y1] - xls[y0]) / (y1 - y0);
      mr = (double) (xrs[y1] - xrs[y0]) / (y1 - y0);

#ifdef IMAGE_DEBUG
      for (; y < Y_MAX; y++) {
        set_point(image, xls[y0] + (int8_t) (ml * (y - y0)), y, BOUND_APP);
        set_point(image, xrs[y0] + (int8_t) (mr * (y - y0)), y, BOUND_APP);
      }
#endif
      state = SearchState::Analyze;
      break;
    }
    case SearchState::Analyze: {
      if (el == LoopLeftBefore2 && ((! l_pad && l_r < 2) || l_stop0)) {
        el = image_result_.element_type = LoopLeft;
        state = SearchState::Loop;
        break;
      }
      if (el == LoopRightBefore2 && ((! r_pad && r_l < 2) || r_stop0)) {
        el = image_result_.element_type = LoopRight;
        state = SearchState::Loop;
        break;
      }

      if (el <= Normal) {
        if (loop_enabled_ && l_segs >= 3 && r_segs == 1) {
          el = image_result_.element_type = LoopLeftBefore;
        }
        else if (loop_enabled_ && l_segs == 1 && r_segs >= 3) {
          el = image_result_.element_type = LoopRightBefore;
        }
        else if (l_segs >= 2 && r_segs >= 2) {
          el = image_result_.element_type = CrossBefore;
        }
        else if (abs(l_convex) < CV_CONVEX && abs(r_convex) < CV_CONVEX) {
          image_result_.element_type = Normal;
        }
        state = SearchState::Mid;
        break;
      }

      if (el == CrossBefore) {
        if (l_segs < 2 && r_segs < 2) {
          el = image_result_.element_type = Cross;
          state = SearchState::CrossBound;
          break;
        }
      }
      state = SearchState::Mid;
      break;
    }
    case SearchState::Mid: {
      so = 0;
      sw = 0;
      int8_t dxm = 0;
      uint8_t both_count = 0;
      for (y = y_end; y >= y_start; y--) {
        xl = xls[y];
        xr = xrs[y];
        if (xl && xr) {
          dx = xr - xl;
          xm = (xl + xr) >> 1;
          dxm = 0;
          both_count++;
        }
        else {
          if (! xl && ! xr) {}
          else if (! xl || el == LoopLeft) {
            dx = kw * STD_WIDTH[y];
            if (both_count > BOTH_COUNT_MIN)
              dxm = xm - (xr - dx);
            else
              xm = xr - dx + dxm;
          }
          else if (! xr || el == LoopRight) {
            dx = kw * STD_WIDTH[y];
            if (both_count > BOTH_COUNT_MIN)
              dxm = xm - (xl + dx);
            else
              xm = xl + dx + dxm;
          }
          both_count = 0;
        }
        so += (xm - X_MID) * dx;
        sw += dx;
        set_point(image, xm, y, MID_LINE);
      }
      image_result_.offset = so / sw;
      set_point(image, X_MID + (int8_t) (image_result_.offset), Y_MAX, SPECIAL);
      state = SearchState::Done;
      break;
    }
    default:
      state = SearchState::Done;
      break;
    }
  }
}

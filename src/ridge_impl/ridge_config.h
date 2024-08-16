#pragma once

namespace sota {

struct RidgeConfig {
  float variation_min_bound{0.0};
  float variation_max_bound{0.1};
  float top_ridge_offset{0.5};
  float bottom_ridge_offset{-0.07};
};

}  // namespace sota

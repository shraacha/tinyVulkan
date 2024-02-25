#pragma once

#include "tv_window.hpp"

namespace tv {

class FirstApp {
public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  void run();

private:
  TvWindow tvWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
};

} // namespace tv

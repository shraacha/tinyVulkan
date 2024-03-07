#pragma once

#include "tv_pipeline.hpp"
#include "tv_window.hpp"

namespace tv {

class FirstApp {
public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  void run();

private:
  TvWindow tvWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
  TvPipeline tvPipeline{"shaders/simple_shader.vert.spv",
                        "shaders/simple_shader.frag.spv"};
};

} // namespace tv

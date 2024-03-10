#pragma once

#include "tv_device.hpp"
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
  TvDevice tvDevice{tvWindow};
  TvPipeline tvPipeline{tvDevice, "shaders/simple_shader.vert.spv",
                        "shaders/simple_shader.frag.spv",
                        TvPipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
};

} // namespace tv

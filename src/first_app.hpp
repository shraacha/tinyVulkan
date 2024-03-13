#pragma once

#include "tv_device.hpp"
#include "tv_pipeline.hpp"
#include "tv_swap_chain.hpp"
#include "tv_window.hpp"
#include "vulkan/vulkan_core.h"

// std
#include <memory>
#include <vector>

namespace tv {

class FirstApp {
public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  FirstApp();
  ~FirstApp();

  FirstApp(const FirstApp &) = delete;
  FirstApp &operator=(const FirstApp &) = delete;

  void run();

private:
  void createPipelineLayout();
  void createPipeline();
  void createCommandBuffers();
  void drawFrame();

  TvWindow m_tvWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
  TvDevice m_tvDevice{m_tvWindow};
  TvSwapChain m_tvSwapChain{m_tvDevice, m_tvWindow.getExtent()};

  std::unique_ptr<TvPipeline> m_tvPipeline;
  VkPipelineLayout m_pipelineLayout;
  std::vector<VkCommandBuffer> m_commandBuffers;
};

} // namespace tv

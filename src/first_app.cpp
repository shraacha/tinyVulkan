#include "first_app.hpp"
#include "tv_pipeline.hpp"
#include "vulkan/vulkan_core.h"

// std
#include <_types/_uint32_t.h>
#include <array>
#include <memory>
#include <stdexcept>

namespace tv {
FirstApp::FirstApp() {
  createPipelineLayout();
  createPipeline();
  createCommandBuffers();
}

FirstApp::~FirstApp() {
  vkDestroyPipelineLayout(m_tvDevice.device(), m_pipelineLayout, nullptr);
}

void FirstApp::run() {
  while (!m_tvWindow.shouldClose()) {
    glfwPollEvents();
    drawFrame();
  }

  // blocks cpu until all gpu operations are completed, otherwise we may end up
  // exiting this func as a frame is being drawn to
  vkDeviceWaitIdle(m_tvDevice.device());
}

void FirstApp::createPipelineLayout() {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;

  if (vkCreatePipelineLayout(m_tvDevice.device(), &pipelineLayoutInfo, nullptr,
                             &m_pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout");
  }
}

void FirstApp::createPipeline() {
  auto pipelineConfigInfo = TvPipeline::defaultPipelineConfigInfo(
      m_tvSwapChain.width(), m_tvSwapChain.height());
  pipelineConfigInfo.renderPass = m_tvSwapChain.getRenderPass();
  pipelineConfigInfo.pipelineLayout = m_pipelineLayout;
  m_tvPipeline = std::make_unique<TvPipeline>(
      m_tvDevice, "shaders/simple_shader.vert.spv",
      "shaders/simple_shader.frag.spv", pipelineConfigInfo);
}

void FirstApp::createCommandBuffers() {
  m_commandBuffers.resize(
      m_tvSwapChain.imageCount()); // 1:1 mapping command buffer to framebuffer

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = m_tvDevice.getCommandPool();
  allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

  if (vkAllocateCommandBuffers(m_tvDevice.device(), &allocInfo,
                               m_commandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers :(");
  }

  for (int i = 0; i < m_commandBuffers.size(); i++) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(m_commandBuffers[i], &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recodring command buffers :(");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_tvSwapChain.getRenderPass();
    renderPassInfo.framebuffer = m_tvSwapChain.getFrameBuffer(i);

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_tvSwapChain.getSwapChainExtent();

    // setting what the inital vals of the framebuffers should clear to
    std::array<VkClearValue, 2> clearValues{};
    // for our framebuffers, index 0 is color attatchment, 1 is depth
    clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(m_commandBuffers[i], &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    m_tvPipeline->bind(m_commandBuffers[i]);
    vkCmdDraw(m_commandBuffers[i], 3, 1, 0, 0);

    vkCmdEndRenderPass(m_commandBuffers[i]);
    if (vkEndCommandBuffer(m_commandBuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer :(");
    }
  }
}

void FirstApp::drawFrame() {
  uint32_t imageIndex;
  VkResult result = m_tvSwapChain.acquireNextImage(&imageIndex);

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to accquire swap chain image :(");
  }

  result = m_tvSwapChain.submitCommandBuffers(&m_commandBuffers[imageIndex],
                                              &imageIndex);

  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image :(");
  }
}
} // namespace tv

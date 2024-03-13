#pragma once

#include "tv_device.hpp"
#include "vulkan/vulkan_core.h"

// std
#include <_types/_uint32_t.h>
#include <string>
#include <vector>

namespace tv {

struct PipelineConfigInfo {
  VkViewport viewport;
  VkRect2D scissor;
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
  VkPipelineRasterizationStateCreateInfo rasterizationInfo;
  VkPipelineMultisampleStateCreateInfo multisampleInfo;
  VkPipelineColorBlendAttachmentState colorBlendAttachment;
  VkPipelineColorBlendStateCreateInfo colorBlendInfo;
  VkPipelineDepthStencilStateCreateInfo depthStencilInfo;

  // no default provided for the following
  VkPipelineLayout pipelineLayout = nullptr;
  VkRenderPass renderPass = nullptr;
  uint32_t subpass = 0;
};

class TvPipeline {
public:
  TvPipeline(TvDevice &device, const std::string &vertFilepath,
             const std::string &fragFilepath,
             const PipelineConfigInfo &configInfo);

  ~TvPipeline();

  // deleting copy ctors because we want to prevent copying of ptrs to our
  // vulkan objects
  TvPipeline(const TvPipeline &) = delete;
  void operator=(const TvPipeline &) = delete;

  void bind(VkCommandBuffer commandBuffer);

  static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width,
                                                      uint32_t height);

private:
  static std::vector<char> readFile(const std::string &filepath);
  void createGraphicsPipeline(const std::string &vertFilepath,
                              const std::string &fragFilepath,
                              const PipelineConfigInfo &configInfo);

  void createShaderModule(const std::vector<char> &code,
                          VkShaderModule *shaderModule);

  // This could be unsafe if we dereference the device after it has been nuked,
  // but the pipeline fundamentally needs the device to exist, so the device
  // this refers to will outlive the pipeline
  TvDevice &m_tvDevice;
  VkPipeline m_graphicsPipeline;
  VkShaderModule m_vertShaderModule;
  VkShaderModule m_fragShaderModule;
};
} // namespace tv

#include "tv_pipeline.hpp"
#include "vulkan/vulkan_core.h"

#include <fstream>
#include <iostream>
#include <stdexcept>

namespace tv {

TvPipeline::TvPipeline(TvDevice &device, const std::string &vertFilepath,
                       const std::string &fragFilepath,
                       const PipelineConfigInfo &configInfo)
    : m_tvDevice{device} {
  createGraphicsPipeline(vertFilepath, fragFilepath, configInfo);
}

std::vector<char> TvPipeline::readFile(const std::string &filepath) {
  std::ifstream file(filepath, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file: " + filepath);
  }

  size_t fileSize = static_cast<size_t>(file.tellg());
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();
  return buffer;
}

PipelineConfigInfo TvPipeline::defaultPipelineConfigInfo(uint32_t width,
                                                         uint32_t height) {
  PipelineConfigInfo configInfo{};

  return configInfo;
}

void TvPipeline::createGraphicsPipeline(const std::string &vertFilepath,
                                        const std::string &fragFilepath,
                                        const PipelineConfigInfo &configInfo) {
  auto vertCode = readFile(vertFilepath);
  auto fragCode = readFile(fragFilepath);

  std::cout << "Vertex shader code size: " << vertCode.size() << "\n";
  std::cout << "Fragment shader code size: " << fragCode.size() << "\n";
}

void TvPipeline::createShaderModule(const std::vector<char> &code,
                                    VkShaderModule *shaderModule) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  // woah woah woah, sizeof(char) != sizeof(uint32_t)...
  // pCode needs the bytecode ptr in uint32_t though,
  // std::vector's default allocator ensures that data satisfies the worst-case
  // allignment requirements
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

  if (vkCreateShaderModule(m_tvDevice.device(), &createInfo, nullptr,
                           shaderModule) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module");
  }
}

} // namespace tv

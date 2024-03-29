#include "tv_pipeline.hpp"
#include "vulkan/vulkan_core.h"

#include <cassert>
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

TvPipeline::~TvPipeline() {
  // *must* destroy our programmable shaders
  vkDestroyShaderModule(m_tvDevice.device(), m_vertShaderModule, nullptr);
  vkDestroyShaderModule(m_tvDevice.device(), m_fragShaderModule, nullptr);

  // *must* destroy the pipeline
  vkDestroyPipeline(m_tvDevice.device(), m_graphicsPipeline, nullptr);
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

void TvPipeline::bind(VkCommandBuffer commandBuffer) {
  // no checks on m_graphicsPipeline bc it must have been constructed properly
  // at instantiation
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    m_graphicsPipeline);
}

PipelineConfigInfo TvPipeline::defaultPipelineConfigInfo(uint32_t width,
                                                         uint32_t height) {
  PipelineConfigInfo configInfo{};

  // ~ input assembly ~
  configInfo.inputAssemblyInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

  // The viewport describes the output area the gl_positions correspond to, and
  // we can set some depth info.
  configInfo.viewport.x = 0.0f;
  configInfo.viewport.y = 0.0f;
  configInfo.viewport.width = static_cast<float>(width);
  configInfo.viewport.height = static_cast<float>(height);
  configInfo.viewport.minDepth = 0.0f;
  configInfo.viewport.maxDepth = 1.0f;

  // Discarding things outside of the scissor.
  configInfo.scissor.offset = {0, 0};
  configInfo.scissor.extent = {width, height};

  // ~ rasterization ~
  configInfo.rasterizationInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  configInfo.rasterizationInfo.depthClampEnable =
      VK_FALSE; // forces the z component of the gl pos to be between 0 & 1
  configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
  configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
  configInfo.rasterizationInfo.lineWidth = 1.0f;
  configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
  configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
  configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
  configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f; // Optional
  configInfo.rasterizationInfo.depthBiasClamp = 0.0f;          // Optional
  configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;    // Optional

  configInfo.multisampleInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
  configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  configInfo.multisampleInfo.minSampleShading = 1.0f;          // Optional
  configInfo.multisampleInfo.pSampleMask = nullptr;            // Optional
  configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE; // Optional
  configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;      // Optional

  configInfo.colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
  configInfo.colorBlendAttachment.srcColorBlendFactor =
      VK_BLEND_FACTOR_ONE; // Optional
  configInfo.colorBlendAttachment.dstColorBlendFactor =
      VK_BLEND_FACTOR_ZERO;                                       // Optional
  configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
  configInfo.colorBlendAttachment.srcAlphaBlendFactor =
      VK_BLEND_FACTOR_ONE; // Optional
  configInfo.colorBlendAttachment.dstAlphaBlendFactor =
      VK_BLEND_FACTOR_ZERO;                                       // Optional
  configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

  configInfo.colorBlendInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
  configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
  configInfo.colorBlendInfo.attachmentCount = 1;
  configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
  configInfo.colorBlendInfo.blendConstants[0] = 0.0f; // Optional
  configInfo.colorBlendInfo.blendConstants[1] = 0.0f; // Optional
  configInfo.colorBlendInfo.blendConstants[2] = 0.0f; // Optional
  configInfo.colorBlendInfo.blendConstants[3] = 0.0f; // Optional

  configInfo.depthStencilInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
  configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
  configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
  configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
  configInfo.depthStencilInfo.minDepthBounds = 0.0f; // Optional
  configInfo.depthStencilInfo.maxDepthBounds = 1.0f; // Optional
  configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
  configInfo.depthStencilInfo.front = {}; // Optional
  configInfo.depthStencilInfo.back = {};  // Optional

  return configInfo;
}

void TvPipeline::createGraphicsPipeline(const std::string &vertFilepath,
                                        const std::string &fragFilepath,
                                        const PipelineConfigInfo &configInfo) {
  assert(configInfo.pipelineLayout != VK_NULL_HANDLE &&
         "Cannot create graphics pipeine:: no pipelineLayout provided in "
         "configInfo");
  assert(
      configInfo.renderPass != VK_NULL_HANDLE &&
      "Cannot create graphics pipeine:: no renderpass provided in configInfo");
  auto vertCode = readFile(vertFilepath);
  auto fragCode = readFile(fragFilepath);

  createShaderModule(vertCode, &m_vertShaderModule);
  createShaderModule(fragCode, &m_fragShaderModule);

  // shader stage info (currently only have the vert and frag shaders)
  VkPipelineShaderStageCreateInfo shaderStages[2];
  shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
  shaderStages[0].module = m_vertShaderModule;
  shaderStages[0].pName = "main"; // entry func
  shaderStages[0].flags = 0;
  shaderStages[0].pNext = nullptr;
  shaderStages[0].pSpecializationInfo =
      nullptr; // a way to customize shader functionality

  shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  shaderStages[1].module = m_fragShaderModule;
  shaderStages[1].pName = "main"; // entry func
  shaderStages[1].flags = 0;
  shaderStages[1].pNext = nullptr;
  shaderStages[1].pSpecializationInfo = nullptr;

  // interpreting the vertex buffer
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexAttributeDescriptionCount = 0;
  vertexInputInfo.vertexBindingDescriptionCount = 0;
  vertexInputInfo.pVertexAttributeDescriptions = nullptr;
  vertexInputInfo.pVertexBindingDescriptions = nullptr;

  // Some graphics cards allow for multiple viewports and scissors.
  // Note, this is not a member of PipelineConfigInfo since it needs pointers to
  // the viewport and the scissor, which would make copies hold ptrs to the old
  // config.
  VkPipelineViewportStateCreateInfo
      viewportInfo{}; // value initializing (important)
  viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportInfo.viewportCount = 1;
  viewportInfo.pViewports = &configInfo.viewport;
  viewportInfo.scissorCount = 1;
  viewportInfo.pScissors = &configInfo.scissor;

  // Finally... creating our pipeline!
  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount =
      2; // number of /programmable/ stages the pipeline will use
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
  pipelineInfo.pViewportState = &viewportInfo;
  pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
  pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
  pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
  pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
  pipelineInfo.pDynamicState = nullptr;

  pipelineInfo.layout = configInfo.pipelineLayout;
  pipelineInfo.renderPass = configInfo.renderPass;
  pipelineInfo.subpass = configInfo.subpass;

  // these can be used to optimize perf by deriving from an existing pipeline
  // than creating a new one
  pipelineInfo.basePipelineIndex = -1;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

  if (vkCreateGraphicsPipelines(m_tvDevice.device(), VK_NULL_HANDLE, 1,
                                &pipelineInfo, nullptr,
                                &m_graphicsPipeline) != VK_SUCCESS) {
    throw std::runtime_error(
        "graphics pipeline failed to be created successfully");
  }
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

#include <np_editor_pch.hpp>
#include <np_core_file_util.hpp>
#include <np_editor_context.hpp>
#include <np_editor_macro.hpp>
#include <np_editor_renderer.hpp>
#include <np_editor_swapchain.hpp>

NpRenderer g_Renderer = {};

NpRenderer::NpRenderer() {}
NpRenderer::~NpRenderer() {}

void NpRenderer::Create(uint32_t FramesInFlight) {
  m_FramesInFlight = FramesInFlight;
  m_FramesInFlight = std::max(m_FramesInFlight, 1U);
  m_FramesInFlight = std::min(m_FramesInFlight, g_Swapchain.GetImageCount());

  CreateCommandBuffer();
  CreateSyncObject();
  CreateDescriptorPool();
  CreateDescriptorSetLayout();
  CreateDescriptorSet();
  CreatePipelineLayout();
  CreateDefaultObjectPipeline();
  CreateDebugLinePipeline();

  CreateTimeBuffer();
  CreateScreenBuffer();
  CreateCameraBuffer();
  CreateDefaultObjectVertexBuffer();
  CreateDefaultObjectIndexBuffer();
  CreateDebugLineVertexBuffer();
  CreateDebugLineIndexBuffer();

  // IMGUI_CREATE();

  UpdateDefaultObjectDescriptorSet();
  UpdateDebugLineDescriptorSet();

  m_DebugLineVertexOffset = new uint32_t[m_FramesInFlight];
  m_DebugLineIndexOffset = new uint32_t[m_FramesInFlight];
}
void NpRenderer::Update() {}
void NpRenderer::Draw(NpTransform *Transform, NpCamera *Camera) {
  vkWaitForFences(g_Context.GetDevice(), 1, &m_FrameFence[m_FrameIndex], 1, UINT64_MAX);

  vkResetFences(g_Context.GetDevice(), 1, &m_FrameFence[m_FrameIndex]);

  vkResetCommandBuffer(m_GraphicsCommandBuffer[m_FrameIndex], 0);

  VkResult AcquireNextImageResult = vkAcquireNextImageKHR(
      g_Context.GetDevice(),
      g_Swapchain.GetSwapchain(),
      UINT64_MAX,
      m_PresentCompleteSemaphore[m_FrameIndex],
      0,
      &m_ImageIndex);

  switch (AcquireNextImageResult) {
    case VK_SUCCESS: {
      break;
    }
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_SUBOPTIMAL_KHR: {
      g_Context.SetSwapchainDirty();

      return;
    }
#if defined(BUILD_DEBUG)
    default: {
      DEBUG_BREAK();
    }
#endif
  }

  UpdateUniformBuffer(Transform, Camera);

  VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
  CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  CommandBufferBeginInfo.pInheritanceInfo = 0;

  vkBeginCommandBuffer(m_GraphicsCommandBuffer[m_FrameIndex], &CommandBufferBeginInfo);

  RecordGraphicsCommand();

  vkEndCommandBuffer(m_GraphicsCommandBuffer[m_FrameIndex]);

  std::array<VkPipelineStageFlags, 2> GraphicsWaitStage = {VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  VkSubmitInfo GraphicsSubmitInfo = {};
  GraphicsSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  GraphicsSubmitInfo.pWaitSemaphores = &m_PresentCompleteSemaphore[m_FrameIndex];
  GraphicsSubmitInfo.waitSemaphoreCount = 1;
  GraphicsSubmitInfo.pSignalSemaphores = &m_GraphicsCompleteSemaphore[m_FrameIndex];
  GraphicsSubmitInfo.signalSemaphoreCount = 1;
  GraphicsSubmitInfo.pCommandBuffers = &m_GraphicsCommandBuffer[m_FrameIndex];
  GraphicsSubmitInfo.commandBufferCount = 1;
  GraphicsSubmitInfo.pWaitDstStageMask = GraphicsWaitStage.data();

  VkResult graphics_queue_submit_result = vkQueueSubmit(g_Context.GetGraphicsQueue(), 1, &GraphicsSubmitInfo, m_FrameFence[m_FrameIndex]);

  VkPresentInfoKHR PresentInfo = {};
  PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  PresentInfo.pWaitSemaphores = &m_GraphicsCompleteSemaphore[m_FrameIndex];
  PresentInfo.waitSemaphoreCount = 1;
  PresentInfo.pSwapchains = &g_Swapchain.GetSwapchain();
  PresentInfo.swapchainCount = 1;
  PresentInfo.pImageIndices = &m_ImageIndex;

  VkResult PresentQueueResult = vkQueuePresentKHR(g_Context.GetPresentQueue(), &PresentInfo);

  switch (PresentQueueResult) {
    case VK_SUCCESS: {
      break;
    }
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_SUBOPTIMAL_KHR: {
      g_Context.SetSwapchainDirty();

      return;
    }
#if defined(BUILD_DEBUG)
    default: {
      DEBUG_BREAK();
    }
#endif
  }

  m_FrameIndex = (m_FrameIndex + 1) % m_FramesInFlight;
}
void NpRenderer::Destroy() {
  VK_CHECK(vkQueueWaitIdle(g_Context.GetGraphicsQueue()));
  VK_CHECK(vkQueueWaitIdle(g_Context.GetPresentQueue()));

  delete[] m_DebugLineVertexOffset;
  delete[] m_DebugLineIndexOffset;

  // IMGUI_DESTROY();

  DestroyDebugLineIndexBuffer();
  DestroyDebugLineVertexBuffer();
  DestroyDefaultObjectIndexBuffer();
  DestroyDefaultObjectVertexBuffer();
  DestroyCameraBuffer();
  DestroyScreenBuffer();
  DestroyTimeBuffer();

  DestroyPipeline();
  DestroyPipelineLayout();
  DestroyDescriptorSet();
  DestroyDescriptorSetLayout();
  DestroyDescriptorPool();
  DestroySyncObject();
  DestroyCommandBuffer();
}

void NpRenderer::CreateCommandBuffer() {
  m_GraphicsCommandBuffer = new VkCommandBuffer[m_FramesInFlight];

  VkCommandBufferAllocateInfo CommandBufferAllocateCreateInfo = {};
  CommandBufferAllocateCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  CommandBufferAllocateCreateInfo.commandPool = g_Context.GetCommandPool();
  CommandBufferAllocateCreateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  CommandBufferAllocateCreateInfo.commandBufferCount = m_FramesInFlight;

  VK_CHECK(vkAllocateCommandBuffers(g_Context.GetDevice(), &CommandBufferAllocateCreateInfo, m_GraphicsCommandBuffer));
}
void NpRenderer::CreateSyncObject() {
  m_GraphicsCompleteSemaphore = new VkSemaphore[m_FramesInFlight];
  m_PresentCompleteSemaphore = new VkSemaphore[m_FramesInFlight];
  m_FrameFence = new VkFence[m_FramesInFlight];

  VkSemaphoreCreateInfo SemaphoreCreateInfo = {};
  SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  SemaphoreCreateInfo.flags = 0;

  VkFenceCreateInfo FenceCreateInfo = {};
  FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  FenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (uint32_t FrameIndex = 0; FrameIndex < m_FramesInFlight; FrameIndex++) {
    VK_CHECK(vkCreateSemaphore(g_Context.GetDevice(), &SemaphoreCreateInfo, nullptr, &m_GraphicsCompleteSemaphore[FrameIndex]));
    VK_CHECK(vkCreateSemaphore(g_Context.GetDevice(), &SemaphoreCreateInfo, nullptr, &m_PresentCompleteSemaphore[FrameIndex]));

    VK_CHECK(vkCreateFence(g_Context.GetDevice(), &FenceCreateInfo, nullptr, &m_FrameFence[FrameIndex]));
  }
}
void NpRenderer::CreateDescriptorPool() {
  std::array<VkDescriptorPoolSize, 1> DefaultObjectDescriptorPoolSizes = {
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1},
  };

  VkDescriptorPoolCreateInfo DefaultObjectDescriptorPoolCreateInfo = {};
  DefaultObjectDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  DefaultObjectDescriptorPoolCreateInfo.pPoolSizes = DefaultObjectDescriptorPoolSizes.data();
  DefaultObjectDescriptorPoolCreateInfo.poolSizeCount = DefaultObjectDescriptorPoolSizes.size();
  DefaultObjectDescriptorPoolCreateInfo.maxSets = m_FramesInFlight;

  VK_CHECK(vkCreateDescriptorPool(g_Context.GetDevice(), &DefaultObjectDescriptorPoolCreateInfo, 0, &m_DefaultObjectDescriptorPool));

  std::array<VkDescriptorPoolSize, 1> DebugLineDescriptorPoolSizes = {
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1},
  };

  VkDescriptorPoolCreateInfo DebugLineDescriptorPoolCreateInfo = {};
  DebugLineDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  DebugLineDescriptorPoolCreateInfo.pPoolSizes = DebugLineDescriptorPoolSizes.data();
  DebugLineDescriptorPoolCreateInfo.poolSizeCount = DebugLineDescriptorPoolSizes.size();
  DebugLineDescriptorPoolCreateInfo.maxSets = m_FramesInFlight;

  VK_CHECK(vkCreateDescriptorPool(g_Context.GetDevice(), &DebugLineDescriptorPoolCreateInfo, 0, &m_DebugLineDescriptorPool));
}
void NpRenderer::CreateDescriptorSetLayout() {
  std::array<VkDescriptorSetLayoutBinding, 1> DefaultObjectDescriptorSetLayoutBindings = {
      {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0},
  };

  VkDescriptorSetLayoutCreateInfo DefaultObjectDescriptorSetLayoutCreateInfo = {};
  DefaultObjectDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  DefaultObjectDescriptorSetLayoutCreateInfo.pBindings = DefaultObjectDescriptorSetLayoutBindings.data();
  DefaultObjectDescriptorSetLayoutCreateInfo.bindingCount = DefaultObjectDescriptorSetLayoutBindings.size();
  DefaultObjectDescriptorSetLayoutCreateInfo.pNext = nullptr;

  VK_CHECK(vkCreateDescriptorSetLayout(g_Context.GetDevice(), &DefaultObjectDescriptorSetLayoutCreateInfo, nullptr, &m_DefaultObjectDescriptorSetLayout));

  std::array<VkDescriptorSetLayoutBinding, 1> DebugLineDescriptorSetLayoutBindings = {
      {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0},
  };

  VkDescriptorSetLayoutCreateInfo DebugLineDescriptorSetLayoutCreateInfo = {};
  DebugLineDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  DebugLineDescriptorSetLayoutCreateInfo.pBindings = DebugLineDescriptorSetLayoutBindings.data();
  DebugLineDescriptorSetLayoutCreateInfo.bindingCount = DebugLineDescriptorSetLayoutBindings.size();
  DebugLineDescriptorSetLayoutCreateInfo.pNext = nullptr;

  VK_CHECK(vkCreateDescriptorSetLayout(g_Context.GetDevice(), &DebugLineDescriptorSetLayoutCreateInfo, nullptr, &m_DebugLineDescriptorSetLayout));
}
void NpRenderer::CreateDescriptorSet() {
  uint32_t DefaultObjectDescriptorSetCount = m_FramesInFlight;
  uint32_t DebugLineDescriptorSetCount = m_FramesInFlight;

  m_DefaultObjectDescriptorSet = new VkDescriptorSet[DefaultObjectDescriptorSetCount];
  m_DebugLineDescriptorSet = new VkDescriptorSet[DebugLineDescriptorSetCount];

  std::vector<VkDescriptorSetLayout> DefaultObjectDescriptorSetLayout(DefaultObjectDescriptorSetCount);
  std::vector<VkDescriptorSetLayout> DebugLineDescriptorSetLayout(DebugLineDescriptorSetCount);

  std::fill_n(DefaultObjectDescriptorSetLayout.begin(), DefaultObjectDescriptorSetLayout.size(), m_DefaultObjectDescriptorSetLayout);
  std::fill_n(DebugLineDescriptorSetLayout.begin(), DebugLineDescriptorSetLayout.size(), m_DebugLineDescriptorSetLayout);

  VkDescriptorSetAllocateInfo DefaultObjectDescriptorSetAllocateInfo = {};
  DefaultObjectDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  DefaultObjectDescriptorSetAllocateInfo.descriptorSetCount = DefaultObjectDescriptorSetCount;
  DefaultObjectDescriptorSetAllocateInfo.descriptorPool = m_DefaultObjectDescriptorPool;
  DefaultObjectDescriptorSetAllocateInfo.pSetLayouts = DefaultObjectDescriptorSetLayout.data();

  VK_CHECK(vkAllocateDescriptorSets(g_Context.GetDevice(), &DefaultObjectDescriptorSetAllocateInfo, m_DefaultObjectDescriptorSet));

  VkDescriptorSetAllocateInfo DebugLineDescriptorSetAllocateInfo = {};
  DebugLineDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  DebugLineDescriptorSetAllocateInfo.descriptorSetCount = DebugLineDescriptorSetCount;
  DebugLineDescriptorSetAllocateInfo.descriptorPool = m_DebugLineDescriptorPool;
  DebugLineDescriptorSetAllocateInfo.pSetLayouts = DebugLineDescriptorSetLayout.data();

  VK_CHECK(vkAllocateDescriptorSets(g_Context.GetDevice(), &DebugLineDescriptorSetAllocateInfo, m_DebugLineDescriptorSet));
}
void NpRenderer::CreatePipelineLayout() {
  VkPipelineLayoutCreateInfo DefaultObjectPipelineLayoutCreateInfo = {};
  DefaultObjectPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  DefaultObjectPipelineLayoutCreateInfo.setLayoutCount = 1;
  DefaultObjectPipelineLayoutCreateInfo.pSetLayouts = &m_DefaultObjectDescriptorSetLayout;
  DefaultObjectPipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
  DefaultObjectPipelineLayoutCreateInfo.pushConstantRangeCount = 0;

  VK_CHECK(vkCreatePipelineLayout(g_Context.GetDevice(), &DefaultObjectPipelineLayoutCreateInfo, nullptr, &m_DefaultObjectPipelineLayout));

  VkPipelineLayoutCreateInfo DebugLinePipelineLayoutCreateInfo = {};
  DebugLinePipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  DebugLinePipelineLayoutCreateInfo.setLayoutCount = 1;
  DebugLinePipelineLayoutCreateInfo.pSetLayouts = &m_DebugLineDescriptorSetLayout;
  DebugLinePipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
  DebugLinePipelineLayoutCreateInfo.pushConstantRangeCount = 0;

  VK_CHECK(vkCreatePipelineLayout(g_Context.GetDevice(), &DebugLinePipelineLayoutCreateInfo, nullptr, &m_DebugLinePipelineLayout));
}

void NpRenderer::CreateDefaultObjectPipeline() {
  std::vector<uint8_t> VertexShaderBytes = {};
  std::vector<uint8_t> FragmentShaderBytes = {};

  NpFileUtil::ReadBinary(m_DefaultObjectVertexShaderFile, VertexShaderBytes);
  NpFileUtil::ReadBinary(m_DefaultObjectFragmentShaderFile, FragmentShaderBytes);

  VkShaderModule VertexModule = nullptr;
  VkShaderModule FragmentModule = nullptr;

  {
    VkShaderModuleCreateInfo ShaderModuleCreateInfo = {};
    ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ShaderModuleCreateInfo.pCode = (uint32_t const *)VertexShaderBytes.data();
    ShaderModuleCreateInfo.codeSize = VertexShaderBytes.size();

    VK_CHECK(vkCreateShaderModule(g_Context.GetDevice(), &ShaderModuleCreateInfo, nullptr, &VertexModule));
  }

  {
    VkShaderModuleCreateInfo ShaderModuleCreateInfo = {};
    ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ShaderModuleCreateInfo.pCode = (uint32_t const *)FragmentShaderBytes.data();
    ShaderModuleCreateInfo.codeSize = FragmentShaderBytes.size();

    VK_CHECK(vkCreateShaderModule(g_Context.GetDevice(), &ShaderModuleCreateInfo, nullptr, &FragmentModule));
  }

  VkPipelineShaderStageCreateInfo VertexShaderStageCreateInfo = {};
  VertexShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  VertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  VertexShaderStageCreateInfo.module = VertexModule;
  VertexShaderStageCreateInfo.pName = "main";

  VkPipelineShaderStageCreateInfo FragmentShaderStageCreateInfo = {};
  FragmentShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  FragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  FragmentShaderStageCreateInfo.module = FragmentModule;
  FragmentShaderStageCreateInfo.pName = "main";

  std::array<VkPipelineShaderStageCreateInfo, 2> ShaderStage = {VertexShaderStageCreateInfo, FragmentShaderStageCreateInfo};

  VkPipelineVertexInputStateCreateInfo VertexInputCreateInfo = {};
  VertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  VertexInputCreateInfo.pVertexBindingDescriptions = m_DefaultObjectVertexInputBindingDescription.data();
  VertexInputCreateInfo.vertexBindingDescriptionCount = m_DefaultObjectVertexInputBindingDescription.size();
  VertexInputCreateInfo.pVertexAttributeDescriptions = m_DefaultObjectVertexInputAttributeDescription.data();
  VertexInputCreateInfo.vertexAttributeDescriptionCount = m_DefaultObjectVertexInputAttributeDescription.size();

  VkPipelineInputAssemblyStateCreateInfo InputAssemblyCreateInfo = {};
  InputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  InputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  InputAssemblyCreateInfo.primitiveRestartEnable = 0;

  VkViewport Viewport = {};
  Viewport.x = 0.0F;
  Viewport.y = 0.0F;
  Viewport.width = (float)g_Context.GetSurfaceWidth();
  Viewport.height = (float)g_Context.GetSurfaceHeight();
  Viewport.minDepth = 0.0F;
  Viewport.maxDepth = 1.0F;

  VkRect2D Scissor = {};
  Scissor.offset.x = 0;
  Scissor.offset.y = 0;
  Scissor.extent.width = g_Context.GetSurfaceWidth();
  Scissor.extent.height = g_Context.GetSurfaceHeight();

  VkPipelineViewportStateCreateInfo ViewportStateCreateInfo = {};
  ViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  ViewportStateCreateInfo.viewportCount = 1;
  ViewportStateCreateInfo.pViewports = &Viewport;
  ViewportStateCreateInfo.scissorCount = 1;
  ViewportStateCreateInfo.pScissors = &Scissor;

  VkPipelineRasterizationStateCreateInfo RasterizerCreateInfo = {};
  RasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  RasterizerCreateInfo.depthClampEnable = 0;
  RasterizerCreateInfo.rasterizerDiscardEnable = 0;
  RasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
  RasterizerCreateInfo.lineWidth = 1.0F;
  RasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
  RasterizerCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
  RasterizerCreateInfo.depthBiasEnable = 0;
  RasterizerCreateInfo.depthBiasConstantFactor = 0.0F;
  RasterizerCreateInfo.depthBiasClamp = 0.0F;
  RasterizerCreateInfo.depthBiasSlopeFactor = 0.0F;

  VkPipelineMultisampleStateCreateInfo MultisamplingCreateInfo = {};
  MultisamplingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  MultisamplingCreateInfo.sampleShadingEnable = 0;
  MultisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  MultisamplingCreateInfo.minSampleShading = 1.0F;
  MultisamplingCreateInfo.pSampleMask = 0;
  MultisamplingCreateInfo.alphaToCoverageEnable = 0;
  MultisamplingCreateInfo.alphaToOneEnable = 0;

  VkPipelineColorBlendAttachmentState ColorBlendAttachment = {};
  ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  ColorBlendAttachment.blendEnable = 1;
  ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineDepthStencilStateCreateInfo DepthStencilStateCreateInfo = {};
  DepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  DepthStencilStateCreateInfo.depthTestEnable = 1;
  DepthStencilStateCreateInfo.depthWriteEnable = 1;
  DepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
  DepthStencilStateCreateInfo.depthBoundsTestEnable = 0;
  DepthStencilStateCreateInfo.stencilTestEnable = 0;

  VkPipelineColorBlendStateCreateInfo ColorBlendCreateInfo = {};
  ColorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  ColorBlendCreateInfo.logicOpEnable = 0;
  ColorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
  ColorBlendCreateInfo.attachmentCount = 1;
  ColorBlendCreateInfo.pAttachments = &ColorBlendAttachment;
  ColorBlendCreateInfo.blendConstants[0] = 0.0F;
  ColorBlendCreateInfo.blendConstants[1] = 0.0F;
  ColorBlendCreateInfo.blendConstants[2] = 0.0F;
  ColorBlendCreateInfo.blendConstants[3] = 0.0F;

  std::array<VkDynamicState, 2> DynamicState = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo DynamicStateCreateInfo = {};
  DynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  DynamicStateCreateInfo.pDynamicStates = DynamicState.data();
  DynamicStateCreateInfo.dynamicStateCount = DynamicState.size();

  VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo = {};
  GraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  GraphicsPipelineCreateInfo.pStages = ShaderStage.data();
  GraphicsPipelineCreateInfo.stageCount = ShaderStage.size();
  GraphicsPipelineCreateInfo.pVertexInputState = &VertexInputCreateInfo;
  GraphicsPipelineCreateInfo.pInputAssemblyState = &InputAssemblyCreateInfo;
  GraphicsPipelineCreateInfo.pViewportState = &ViewportStateCreateInfo;
  GraphicsPipelineCreateInfo.pRasterizationState = &RasterizerCreateInfo;
  GraphicsPipelineCreateInfo.pMultisampleState = &MultisamplingCreateInfo;
  GraphicsPipelineCreateInfo.pDepthStencilState = &DepthStencilStateCreateInfo;
  GraphicsPipelineCreateInfo.pColorBlendState = &ColorBlendCreateInfo;
  GraphicsPipelineCreateInfo.pDynamicState = &DynamicStateCreateInfo;
  GraphicsPipelineCreateInfo.layout = m_DefaultObjectPipelineLayout;
  GraphicsPipelineCreateInfo.renderPass = g_Swapchain.GetRenderPass();
  GraphicsPipelineCreateInfo.subpass = 0;
  GraphicsPipelineCreateInfo.basePipelineHandle = nullptr;

  VK_CHECK(vkCreateGraphicsPipelines(g_Context.GetDevice(), nullptr, 1, &GraphicsPipelineCreateInfo, nullptr, &m_DefaultObjectPipeline));

  vkDestroyShaderModule(g_Context.GetDevice(), VertexModule, nullptr);
  vkDestroyShaderModule(g_Context.GetDevice(), FragmentModule, nullptr);
}
void NpRenderer::CreateDebugLinePipeline() {
  std::vector<uint8_t> VertexShaderBytes = {};
  std::vector<uint8_t> FragmentShaderBytes = {};

  NpFileUtil::ReadBinary(m_DebugLineVertexShaderFile, VertexShaderBytes);
  NpFileUtil::ReadBinary(m_DebugLineFragmentShaderFile, FragmentShaderBytes);

  VkShaderModule VertexModule = nullptr;
  VkShaderModule FragmentModule = nullptr;

  {
    VkShaderModuleCreateInfo ShaderModuleCreateInfo = {};
    ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ShaderModuleCreateInfo.pCode = (uint32_t const *)VertexShaderBytes.data();
    ShaderModuleCreateInfo.codeSize = VertexShaderBytes.size();

    VK_CHECK(vkCreateShaderModule(g_Context.GetDevice(), &ShaderModuleCreateInfo, nullptr, &VertexModule));
  }

  {
    VkShaderModuleCreateInfo ShaderModuleCreateInfo = {};
    ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ShaderModuleCreateInfo.pCode = (uint32_t const *)FragmentShaderBytes.data();
    ShaderModuleCreateInfo.codeSize = FragmentShaderBytes.size();

    VK_CHECK(vkCreateShaderModule(g_Context.GetDevice(), &ShaderModuleCreateInfo, nullptr, &FragmentModule));
  }

  VkPipelineShaderStageCreateInfo VertexShaderStageCreateInfo = {};
  VertexShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  VertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  VertexShaderStageCreateInfo.module = VertexModule;
  VertexShaderStageCreateInfo.pName = "main";

  VkPipelineShaderStageCreateInfo FragmentShaderStageCreateInfo = {};
  FragmentShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  FragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  FragmentShaderStageCreateInfo.module = FragmentModule;
  FragmentShaderStageCreateInfo.pName = "main";

  std::array<VkPipelineShaderStageCreateInfo, 2> ShaderStage = {VertexShaderStageCreateInfo, FragmentShaderStageCreateInfo};

  VkPipelineVertexInputStateCreateInfo VertexInputCreateInfo = {};
  VertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  VertexInputCreateInfo.pVertexBindingDescriptions = m_DebugLineVertexInputBindingDescription.data();
  VertexInputCreateInfo.vertexBindingDescriptionCount = m_DebugLineVertexInputBindingDescription.size();
  VertexInputCreateInfo.pVertexAttributeDescriptions = m_DebugLineVertexInputAttributeDescription.data();
  VertexInputCreateInfo.vertexAttributeDescriptionCount = m_DebugLineVertexInputAttributeDescription.size();

  VkPipelineInputAssemblyStateCreateInfo InputAssemblyCreateInfo = {};
  InputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  InputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
  InputAssemblyCreateInfo.primitiveRestartEnable = 0;

  VkViewport Viewport = {};
  Viewport.x = 0.0F;
  Viewport.y = 0.0F;
  Viewport.width = (float)g_Context.GetSurfaceWidth();
  Viewport.height = (float)g_Context.GetSurfaceHeight();
  Viewport.minDepth = 0.0F;
  Viewport.maxDepth = 1.0F;

  VkRect2D Scissor = {};
  Scissor.offset.x = 0;
  Scissor.offset.y = 0;
  Scissor.extent.width = g_Context.GetSurfaceWidth();
  Scissor.extent.height = g_Context.GetSurfaceHeight();

  VkPipelineViewportStateCreateInfo ViewportStateCreateInfo = {};
  ViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  ViewportStateCreateInfo.viewportCount = 1;
  ViewportStateCreateInfo.pViewports = &Viewport;
  ViewportStateCreateInfo.scissorCount = 1;
  ViewportStateCreateInfo.pScissors = &Scissor;

  VkPipelineRasterizationStateCreateInfo RasterizerCreateInfo = {};
  RasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  RasterizerCreateInfo.depthClampEnable = 0;
  RasterizerCreateInfo.rasterizerDiscardEnable = 0;
  RasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
  RasterizerCreateInfo.lineWidth = 1.0F;
  RasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
  RasterizerCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
  RasterizerCreateInfo.depthBiasEnable = 0;
  RasterizerCreateInfo.depthBiasConstantFactor = 0.0F;
  RasterizerCreateInfo.depthBiasClamp = 0.0F;
  RasterizerCreateInfo.depthBiasSlopeFactor = 0.0F;

  VkPipelineMultisampleStateCreateInfo MultisamplingCreateInfo = {};
  MultisamplingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  MultisamplingCreateInfo.sampleShadingEnable = 0;
  MultisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  MultisamplingCreateInfo.minSampleShading = 1.0F;
  MultisamplingCreateInfo.pSampleMask = 0;
  MultisamplingCreateInfo.alphaToCoverageEnable = 0;
  MultisamplingCreateInfo.alphaToOneEnable = 0;

  VkPipelineColorBlendAttachmentState ColorBlendAttachment = {};
  ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  ColorBlendAttachment.blendEnable = 0;
  ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineDepthStencilStateCreateInfo DepthStencilStateCreateInfo = {};
  DepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  DepthStencilStateCreateInfo.depthTestEnable = 0;
  DepthStencilStateCreateInfo.depthWriteEnable = 0;
  DepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
  DepthStencilStateCreateInfo.depthBoundsTestEnable = 0;
  DepthStencilStateCreateInfo.stencilTestEnable = 0;

  VkPipelineColorBlendStateCreateInfo ColorBlendCreateInfo = {};
  ColorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  ColorBlendCreateInfo.logicOpEnable = 0;
  ColorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
  ColorBlendCreateInfo.attachmentCount = 1;
  ColorBlendCreateInfo.pAttachments = &ColorBlendAttachment;
  ColorBlendCreateInfo.blendConstants[0] = 0.0F;
  ColorBlendCreateInfo.blendConstants[1] = 0.0F;
  ColorBlendCreateInfo.blendConstants[2] = 0.0F;
  ColorBlendCreateInfo.blendConstants[3] = 0.0F;

  std::array<VkDynamicState, 2> DynamicState = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo DynamicStateCreateInfo = {};
  DynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  DynamicStateCreateInfo.pDynamicStates = DynamicState.data();
  DynamicStateCreateInfo.dynamicStateCount = DynamicState.size();

  VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo = {};
  GraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  GraphicsPipelineCreateInfo.pStages = ShaderStage.data();
  GraphicsPipelineCreateInfo.stageCount = ShaderStage.size();
  GraphicsPipelineCreateInfo.pVertexInputState = &VertexInputCreateInfo;
  GraphicsPipelineCreateInfo.pInputAssemblyState = &InputAssemblyCreateInfo;
  GraphicsPipelineCreateInfo.pViewportState = &ViewportStateCreateInfo;
  GraphicsPipelineCreateInfo.pRasterizationState = &RasterizerCreateInfo;
  GraphicsPipelineCreateInfo.pMultisampleState = &MultisamplingCreateInfo;
  GraphicsPipelineCreateInfo.pDepthStencilState = &DepthStencilStateCreateInfo;
  GraphicsPipelineCreateInfo.pColorBlendState = &ColorBlendCreateInfo;
  GraphicsPipelineCreateInfo.pDynamicState = &DynamicStateCreateInfo;
  GraphicsPipelineCreateInfo.layout = m_DebugLinePipelineLayout;
  GraphicsPipelineCreateInfo.renderPass = g_Swapchain.GetRenderPass();
  GraphicsPipelineCreateInfo.subpass = 0;
  GraphicsPipelineCreateInfo.basePipelineHandle = nullptr;

  VK_CHECK(vkCreateGraphicsPipelines(g_Context.GetDevice(), nullptr, 1, &GraphicsPipelineCreateInfo, nullptr, &m_DebugLinePipeline));

  vkDestroyShaderModule(g_Context.GetDevice(), VertexModule, nullptr);
  vkDestroyShaderModule(g_Context.GetDevice(), FragmentModule, nullptr);
}

void NpRenderer::CreateTimeBuffer() {}
void NpRenderer::CreateScreenBuffer() {}
void NpRenderer::CreateCameraBuffer() {}
void NpRenderer::CreateDefaultObjectVertexBuffer() {}
void NpRenderer::CreateDefaultObjectIndexBuffer() {}
void NpRenderer::CreateDebugLineVertexBuffer() {}
void NpRenderer::CreateDebugLineIndexBuffer() {}

void NpRenderer::UpdateDefaultObjectDescriptorSet() {}
void NpRenderer::UpdateDebugLineDescriptorSet() {}

void NpRenderer::UpdateUniformBuffer(NpTransform *Transform, NpCamera *Camera) {}

void NpRenderer::RecordGraphicsCommand() {}

void NpRenderer::DestroyCommandBuffer() {}
void NpRenderer::DestroySyncObject() {}
void NpRenderer::DestroyDescriptorPool() {}
void NpRenderer::DestroyDescriptorSetLayout() {}
void NpRenderer::DestroyDescriptorSet() {}
void NpRenderer::DestroyPipelineLayout() {}
void NpRenderer::DestroyPipeline() {}

void NpRenderer::DestroyTimeBuffer() {}
void NpRenderer::DestroyScreenBuffer() {}
void NpRenderer::DestroyCameraBuffer() {}
void NpRenderer::DestroyDefaultObjectVertexBuffer() {}
void NpRenderer::DestroyDefaultObjectIndexBuffer() {}
void NpRenderer::DestroyDebugLineVertexBuffer() {}
void NpRenderer::DestroyDebugLineIndexBuffer() {}

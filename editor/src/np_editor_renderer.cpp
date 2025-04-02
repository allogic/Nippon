#include <np_editor_pch.hpp>
#include <np_core_file_util.hpp>
#include <np_editor_constants.hpp>
#include <np_editor_context.hpp>
#include <np_editor_macro.hpp>
#include <np_editor_renderer.hpp>
#include <np_editor_swapchain.hpp>

// #define NP_FORCE_INLINE_CONTENT

#define NP_RENDERER_DEFAULT_OBJECT_VERTEX_SHADER_SECTION_NAME ".dovs"
#define NP_RENDERER_DEFAULT_OBJECT_VERTEX_SHADER_SECTION_SIZE (8192ULL)

#define NP_RENDERER_DEFAULT_OBJECT_FRAGMENT_SHADER_SECTION_NAME ".dofs"
#define NP_RENDERER_DEFAULT_OBJECT_FRAGMENT_SHADER_SECTION_SIZE (8192ULL)

#define NP_RENDERER_DEBUG_LINE_VERTEX_SHADER_SECTION_NAME ".dlvs"
#define NP_RENDERER_DEBUG_LINE_VERTEX_SHADER_SECTION_SIZE (8192ULL)

#define NP_RENDERER_DEBUG_LINE_FRAGMENT_SHADER_SECTION_NAME ".dlfs"
#define NP_RENDERER_DEBUG_LINE_FRAGMENT_SHADER_SECTION_SIZE (8192ULL)

#if defined(OS_WINDOWS)
#  pragma section(NP_RENDERER_DEFAULT_OBJECT_VERTEX_SHADER_SECTION_NAME, read)
#  pragma section(NP_RENDERER_DEFAULT_OBJECT_FRAGMENT_SHADER_SECTION_NAME, read)
#  pragma section(NP_RENDERER_DEBUG_LINE_VERTEX_SHADER_SECTION_NAME, read)
#  pragma section(NP_RENDERER_DEBUG_LINE_FRAGMENT_SHADER_SECTION_NAME, read)

__declspec(allocate(NP_RENDERER_DEFAULT_OBJECT_VERTEX_SHADER_SECTION_NAME)) static char const g_RendererDefaultObjectVertexShader[NP_RENDERER_DEFAULT_OBJECT_VERTEX_SHADER_SECTION_SIZE];
__declspec(allocate(NP_RENDERER_DEFAULT_OBJECT_FRAGMENT_SHADER_SECTION_NAME)) static char const g_RendererDefaultObjectFragmentShader[NP_RENDERER_DEFAULT_OBJECT_FRAGMENT_SHADER_SECTION_SIZE];
__declspec(allocate(NP_RENDERER_DEBUG_LINE_VERTEX_SHADER_SECTION_NAME)) static char const g_RendererDebugLineVertexShader[NP_RENDERER_DEBUG_LINE_VERTEX_SHADER_SECTION_SIZE];
__declspec(allocate(NP_RENDERER_DEBUG_LINE_FRAGMENT_SHADER_SECTION_NAME)) static char const g_RendererDebugLineFragmentShader[NP_RENDERER_DEBUG_LINE_FRAGMENT_SHADER_SECTION_SIZE];
#else defined(OS_LINUX)
__attribute__((section(NP_RENDERER_DEFAULT_OBJECT_VERTEX_SHADER_SECTION_NAME))) static char const g_RendererDefaultObjectVertexShader[NP_RENDERER_DEFAULT_OBJECT_VERTEX_SHADER_SECTION_SIZE];
__attribute__((section(NP_RENDERER_DEFAULT_OBJECT_FRAGMENT_SHADER_SECTION_NAME))) static char const g_RendererDefaultObjectFragmentShader[NP_RENDERER_DEFAULT_OBJECT_FRAGMENT_SHADER_SECTION_SIZE];
__attribute__((section(NP_RENDERER_DEBUG_LINE_VERTEX_SHADER_SECTION_NAME))) static char const g_RendererDebugLineVertexShader[NP_RENDERER_DEBUG_LINE_VERTEX_SHADER_SECTION_SIZE];
__attribute__((section(NP_RENDERER_DEBUG_LINE_FRAGMENT_SHADER_SECTION_NAME))) static char const g_RendererDebugLineFragmentShader[NP_RENDERER_DEBUG_LINE_FRAGMENT_SHADER_SECTION_SIZE];
#endif

#define NP_RENDERER_DEBUG_LINE_VERTEX_COUNT (1048576ULL)
#define NP_RENDERER_DEBUG_LINE_INDEX_COUNT (1048576ULL)

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
#if BUILD_DEBUG
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
#if BUILD_DEBUG
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

void NpRenderer::DrawDebugLine(glm::fvec3 const &From, glm::fvec3 const &To, glm::fvec4 const &Color) {
  if (m_EnableDebug) {
    m_DebugLineVertex[m_FrameIndex][m_DebugLineVertexOffset[m_FrameIndex] + 0].Position = From;
    m_DebugLineVertex[m_FrameIndex][m_DebugLineVertexOffset[m_FrameIndex] + 1].Position = To;

    m_DebugLineVertex[m_FrameIndex][m_DebugLineVertexOffset[m_FrameIndex] + 0].Color = Color;
    m_DebugLineVertex[m_FrameIndex][m_DebugLineVertexOffset[m_FrameIndex] + 1].Color = Color;

    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 0] = m_DebugLineVertexOffset[m_FrameIndex] + 0;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 1] = m_DebugLineVertexOffset[m_FrameIndex] + 1;

    m_DebugLineVertexOffset[m_FrameIndex] += 2;
    m_DebugLineIndexOffset[m_FrameIndex] += 2;
  }
}
void NpRenderer::DrawDebugBox(glm::fvec3 const &Position, glm::fvec3 const &Size, glm::fvec4 const &Color) {
  if (m_EnableDebug) {
    m_DebugLineVertex[m_FrameIndex][m_DebugLineVertexOffset[m_FrameIndex] + 0].Position = {Position.x, Position.y, Position.z};
    m_DebugLineVertex[m_FrameIndex][m_DebugLineVertexOffset[m_FrameIndex] + 1].Position = {Position.x, Position.y + Size.y, Position.z};
    m_DebugLineVertex[m_FrameIndex][m_DebugLineVertexOffset[m_FrameIndex] + 2].Position = {Position.x + Size.x, Position.y, Position.z};
    m_DebugLineVertex[m_FrameIndex][m_DebugLineVertexOffset[m_FrameIndex] + 3].Position = {Position.x + Size.x, Position.y + Size.y, Position.z};
    m_DebugLineVertex[m_FrameIndex][m_DebugLineVertexOffset[m_FrameIndex] + 4].Position = {Position.x, Position.y, Position.z + Size.z};
    m_DebugLineVertex[m_FrameIndex][m_DebugLineVertexOffset[m_FrameIndex] + 5].Position = {Position.x, Position.y + Size.y, Position.z + Size.z};
    m_DebugLineVertex[m_FrameIndex][m_DebugLineVertexOffset[m_FrameIndex] + 6].Position = {Position.x + Size.x, Position.y, Position.z + Size.z};
    m_DebugLineVertex[m_FrameIndex][m_DebugLineVertexOffset[m_FrameIndex] + 7].Position = {Position.x + Size.x, Position.y + Size.y, Position.z + Size.z};

    m_DebugLineVertex[m_FrameIndex][m_DebugLineVertexOffset[m_FrameIndex] + 0].Color = Color;
    m_DebugLineVertex[m_FrameIndex][m_DebugLineVertexOffset[m_FrameIndex] + 1].Color = Color;
    m_DebugLineVertex[m_FrameIndex][m_DebugLineVertexOffset[m_FrameIndex] + 2].Color = Color;
    m_DebugLineVertex[m_FrameIndex][m_DebugLineVertexOffset[m_FrameIndex] + 3].Color = Color;
    m_DebugLineVertex[m_FrameIndex][m_DebugLineVertexOffset[m_FrameIndex] + 4].Color = Color;
    m_DebugLineVertex[m_FrameIndex][m_DebugLineVertexOffset[m_FrameIndex] + 5].Color = Color;
    m_DebugLineVertex[m_FrameIndex][m_DebugLineVertexOffset[m_FrameIndex] + 6].Color = Color;
    m_DebugLineVertex[m_FrameIndex][m_DebugLineVertexOffset[m_FrameIndex] + 7].Color = Color;

    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 0] = m_DebugLineVertexOffset[m_FrameIndex] + 0;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 1] = m_DebugLineVertexOffset[m_FrameIndex] + 1;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 2] = m_DebugLineVertexOffset[m_FrameIndex] + 1;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 3] = m_DebugLineVertexOffset[m_FrameIndex] + 3;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 4] = m_DebugLineVertexOffset[m_FrameIndex] + 3;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 5] = m_DebugLineVertexOffset[m_FrameIndex] + 2;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 6] = m_DebugLineVertexOffset[m_FrameIndex] + 2;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 7] = m_DebugLineVertexOffset[m_FrameIndex] + 0;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 8] = m_DebugLineVertexOffset[m_FrameIndex] + 4;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 9] = m_DebugLineVertexOffset[m_FrameIndex] + 5;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 10] = m_DebugLineVertexOffset[m_FrameIndex] + 5;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 11] = m_DebugLineVertexOffset[m_FrameIndex] + 7;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 12] = m_DebugLineVertexOffset[m_FrameIndex] + 7;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 13] = m_DebugLineVertexOffset[m_FrameIndex] + 6;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 14] = m_DebugLineVertexOffset[m_FrameIndex] + 6;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 15] = m_DebugLineVertexOffset[m_FrameIndex] + 4;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 16] = m_DebugLineVertexOffset[m_FrameIndex] + 0;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 17] = m_DebugLineVertexOffset[m_FrameIndex] + 4;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 18] = m_DebugLineVertexOffset[m_FrameIndex] + 1;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 19] = m_DebugLineVertexOffset[m_FrameIndex] + 5;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 20] = m_DebugLineVertexOffset[m_FrameIndex] + 2;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 21] = m_DebugLineVertexOffset[m_FrameIndex] + 6;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 22] = m_DebugLineVertexOffset[m_FrameIndex] + 3;
    m_DebugLineIndex[m_FrameIndex][m_DebugLineIndexOffset[m_FrameIndex] + 23] = m_DebugLineVertexOffset[m_FrameIndex] + 7;

    m_DebugLineVertexOffset[m_FrameIndex] += 8;
    m_DebugLineIndexOffset[m_FrameIndex] += 24;
  }
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
  uint32_t const *VertexShader = nullptr;
  uint32_t const *FragmentShader = nullptr;

  uint64_t VertexShaderSize = 0;
  uint64_t FragmentShaderSize = 0;

#if BUILD_DEBUG && !defined(NP_FORCE_INLINE_CONTENT)
  std::vector<uint8_t> VertexShaderBytes = {};
  std::vector<uint8_t> FragmentShaderBytes = {};

  NpFileUtil::ReadBinary(std::filesystem::path(NP_ROOT_DIR) / "np_shader_default_object.vert.spv", VertexShaderBytes);
  NpFileUtil::ReadBinary(std::filesystem::path(NP_ROOT_DIR) / "np_shader_default_object.frag.spv", FragmentShaderBytes);

  VertexShader = (uint32_t const *)VertexShaderBytes.data();
  FragmentShader = (uint32_t const *)FragmentShaderBytes.data();

  VertexShaderSize = VertexShaderBytes.size();
  FragmentShaderSize = FragmentShaderBytes.size();
#else
  VertexShader = (uint32_t const *)g_RendererDefaultObjectVertexShader;
  FragmentShader = (uint32_t const *)g_RendererDefaultObjectFragmentShader;

  VertexShaderSize = sizeof(g_RendererDefaultObjectVertexShader);
  FragmentShaderSize = sizeof(g_RendererDefaultObjectFragmentShader);
#endif

  VkShaderModule VertexModule = nullptr;
  VkShaderModule FragmentModule = nullptr;

  {
    VkShaderModuleCreateInfo ShaderModuleCreateInfo = {};
    ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ShaderModuleCreateInfo.pCode = VertexShader;
    ShaderModuleCreateInfo.codeSize = VertexShaderSize;

    VK_CHECK(vkCreateShaderModule(g_Context.GetDevice(), &ShaderModuleCreateInfo, nullptr, &VertexModule));
  }

  {
    VkShaderModuleCreateInfo ShaderModuleCreateInfo = {};
    ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ShaderModuleCreateInfo.pCode = FragmentShader;
    ShaderModuleCreateInfo.codeSize = FragmentShaderSize;

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
  uint32_t const *VertexShader = nullptr;
  uint32_t const *FragmentShader = nullptr;

  uint64_t VertexShaderSize = 0;
  uint64_t FragmentShaderSize = 0;

#if BUILD_DEBUG && !defined(NP_FORCE_INLINE_CONTENT)
  std::vector<uint8_t> VertexShaderBytes = {};
  std::vector<uint8_t> FragmentShaderBytes = {};

  NpFileUtil::ReadBinary(std::filesystem::path(NP_ROOT_DIR) / "np_shader_debug_line.vert.spv", VertexShaderBytes);
  NpFileUtil::ReadBinary(std::filesystem::path(NP_ROOT_DIR) / "np_shader_debug_line.frag.spv", FragmentShaderBytes);

  VertexShader = (uint32_t const *)VertexShaderBytes.data();
  FragmentShader = (uint32_t const *)FragmentShaderBytes.data();

  VertexShaderSize = VertexShaderBytes.size();
  FragmentShaderSize = FragmentShaderBytes.size();
#else
  VertexShader = (uint32_t const *)g_RendererDebugLineVertexShader;
  FragmentShader = (uint32_t const *)g_RendererDebugLineFragmentShader;

  VertexShaderSize = sizeof(g_RendererDebugLineVertexShader);
  FragmentShaderSize = sizeof(g_RendererDebugLineFragmentShader);
#endif

  VkShaderModule VertexModule = nullptr;
  VkShaderModule FragmentModule = nullptr;

  {
    VkShaderModuleCreateInfo ShaderModuleCreateInfo = {};
    ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ShaderModuleCreateInfo.pCode = VertexShader;
    ShaderModuleCreateInfo.codeSize = VertexShaderSize;

    VK_CHECK(vkCreateShaderModule(g_Context.GetDevice(), &ShaderModuleCreateInfo, nullptr, &VertexModule));
  }

  {
    VkShaderModuleCreateInfo ShaderModuleCreateInfo = {};
    ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ShaderModuleCreateInfo.pCode = FragmentShader;
    ShaderModuleCreateInfo.codeSize = FragmentShaderSize;

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

void NpRenderer::CreateTimeBuffer() {
  m_TimeBuffer = new VkBuffer[m_FramesInFlight];
  m_TimeBufferDeviceMemory = new VkDeviceMemory[m_FramesInFlight];
  m_Time = new NpTimeInfo *[m_FramesInFlight];

  for (uint32_t FrameIndex = 0; FrameIndex < m_FramesInFlight; FrameIndex++) {
    VkDeviceSize Size = sizeof(NpTimeInfo);

    VkBufferCreateInfo BufferCreateInfo = {};
    BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    BufferCreateInfo.size = Size;
    BufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateBuffer(g_Context.GetDevice(), &BufferCreateInfo, nullptr, &m_TimeBuffer[FrameIndex]));

    VkMemoryRequirements MemoryRequirements = {};

    vkGetBufferMemoryRequirements(g_Context.GetDevice(), m_TimeBuffer[FrameIndex], &MemoryRequirements);

    VkMemoryAllocateInfo MemoryAllocateInfo = {};
    MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
    MemoryAllocateInfo.memoryTypeIndex = g_Context.FindMemoryType(MemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT);

    VK_CHECK(vkAllocateMemory(g_Context.GetDevice(), &MemoryAllocateInfo, nullptr, &m_TimeBufferDeviceMemory[FrameIndex]));
    VK_CHECK(vkBindBufferMemory(g_Context.GetDevice(), m_TimeBuffer[FrameIndex], m_TimeBufferDeviceMemory[FrameIndex], 0));
    VK_CHECK(vkMapMemory(g_Context.GetDevice(), m_TimeBufferDeviceMemory[FrameIndex], 0, Size, 0, (void **)&m_Time[FrameIndex]));
  }
}
void NpRenderer::CreateScreenBuffer() {
  m_ScreenBuffer = new VkBuffer[m_FramesInFlight];
  m_ScreenBufferDeviceMemory = new VkDeviceMemory[m_FramesInFlight];
  m_Screen = new NpScreenInfo *[m_FramesInFlight];

  for (uint32_t FrameIndex = 0; FrameIndex < m_FramesInFlight; FrameIndex++) {
    VkDeviceSize Size = sizeof(NpScreenInfo);

    VkBufferCreateInfo BufferCreateInfo = {};
    BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    BufferCreateInfo.size = Size;
    BufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateBuffer(g_Context.GetDevice(), &BufferCreateInfo, nullptr, &m_ScreenBuffer[FrameIndex]));

    VkMemoryRequirements MemoryRequirements = {};

    vkGetBufferMemoryRequirements(g_Context.GetDevice(), m_ScreenBuffer[FrameIndex], &MemoryRequirements);

    VkMemoryAllocateInfo MemoryAllocateInfo = {};
    MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
    MemoryAllocateInfo.memoryTypeIndex = g_Context.FindMemoryType(MemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT);

    VK_CHECK(vkAllocateMemory(g_Context.GetDevice(), &MemoryAllocateInfo, nullptr, &m_ScreenBufferDeviceMemory[FrameIndex]));
    VK_CHECK(vkBindBufferMemory(g_Context.GetDevice(), m_ScreenBuffer[FrameIndex], m_ScreenBufferDeviceMemory[FrameIndex], 0));
    VK_CHECK(vkMapMemory(g_Context.GetDevice(), m_ScreenBufferDeviceMemory[FrameIndex], 0, Size, 0, (void **)&m_Screen[FrameIndex]));
  }
}
void NpRenderer::CreateCameraBuffer() {
  m_CameraBuffer = new VkBuffer[m_FramesInFlight];
  m_CameraBufferDeviceMemory = new VkDeviceMemory[m_FramesInFlight];
  m_Camera = new NpCameraInfo *[m_FramesInFlight];

  for (uint32_t FrameIndex = 0; FrameIndex < m_FramesInFlight; FrameIndex++) {
    VkDeviceSize Size = sizeof(NpCameraInfo);

    VkBufferCreateInfo BufferCreateInfo = {};
    BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    BufferCreateInfo.size = Size;
    BufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateBuffer(g_Context.GetDevice(), &BufferCreateInfo, nullptr, &m_CameraBuffer[FrameIndex]));

    VkMemoryRequirements MemoryRequirements = {};

    vkGetBufferMemoryRequirements(g_Context.GetDevice(), m_CameraBuffer[FrameIndex], &MemoryRequirements);

    VkMemoryAllocateInfo MemoryAllocateInfo = {};
    MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
    MemoryAllocateInfo.memoryTypeIndex = g_Context.FindMemoryType(MemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT);

    VK_CHECK(vkAllocateMemory(g_Context.GetDevice(), &MemoryAllocateInfo, nullptr, &m_CameraBufferDeviceMemory[FrameIndex]));
    VK_CHECK(vkBindBufferMemory(g_Context.GetDevice(), m_CameraBuffer[FrameIndex], m_CameraBufferDeviceMemory[FrameIndex], 0));
    VK_CHECK(vkMapMemory(g_Context.GetDevice(), m_CameraBufferDeviceMemory[FrameIndex], 0, Size, 0, (void **)&m_Camera[FrameIndex]));
  }
}
void NpRenderer::CreateDefaultObjectVertexBuffer() { /* TODO */ }
void NpRenderer::CreateDefaultObjectIndexBuffer() { /* TODO */ }
void NpRenderer::CreateDebugLineVertexBuffer() {
  m_DebugLineVertexBuffer = new VkBuffer[m_FramesInFlight];
  m_DebugLineVertexBufferDeviceMemory = new VkDeviceMemory[m_FramesInFlight];
  m_DebugLineVertex = new NpDebugLineVertex *[m_FramesInFlight];

  for (uint32_t FrameIndex = 0; FrameIndex < m_FramesInFlight; FrameIndex++) {
    VkDeviceSize Size = sizeof(NpDebugLineVertex) * NP_RENDERER_DEBUG_LINE_VERTEX_COUNT;

    VkBufferCreateInfo BufferCreateInfo = {};
    BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    BufferCreateInfo.size = Size;
    BufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateBuffer(g_Context.GetDevice(), &BufferCreateInfo, nullptr, &m_DebugLineVertexBuffer[FrameIndex]));

    VkMemoryRequirements MemoryRequirements = {};

    vkGetBufferMemoryRequirements(g_Context.GetDevice(), m_DebugLineVertexBuffer[FrameIndex], &MemoryRequirements);

    VkMemoryAllocateInfo MemoryAllocateInfo = {};
    MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
    MemoryAllocateInfo.memoryTypeIndex = g_Context.FindMemoryType(MemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT);

    VK_CHECK(vkAllocateMemory(g_Context.GetDevice(), &MemoryAllocateInfo, nullptr, &m_DebugLineVertexBufferDeviceMemory[FrameIndex]));
    VK_CHECK(vkBindBufferMemory(g_Context.GetDevice(), m_DebugLineVertexBuffer[FrameIndex], m_DebugLineVertexBufferDeviceMemory[FrameIndex], 0));
    VK_CHECK(vkMapMemory(g_Context.GetDevice(), m_DebugLineVertexBufferDeviceMemory[FrameIndex], 0, Size, 0, (void **)&m_DebugLineVertex[FrameIndex]));
  }
}
void NpRenderer::CreateDebugLineIndexBuffer() {
  m_DebugLineIndexBuffer = new VkBuffer[m_FramesInFlight];
  m_DebugLineIndexBufferDeviceMemory = new VkDeviceMemory[m_FramesInFlight];
  m_DebugLineIndex = new NpDebugLineIndex *[m_FramesInFlight];

  for (uint32_t FrameIndex = 0; FrameIndex < m_FramesInFlight; FrameIndex++) {
    VkDeviceSize Size = sizeof(NpDebugLineIndex) * NP_RENDERER_DEBUG_LINE_INDEX_COUNT;

    VkBufferCreateInfo BufferCreateInfo = {};
    BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    BufferCreateInfo.size = Size;
    BufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateBuffer(g_Context.GetDevice(), &BufferCreateInfo, nullptr, &m_DebugLineIndexBuffer[FrameIndex]));

    VkMemoryRequirements MemoryRequirements = {};

    vkGetBufferMemoryRequirements(g_Context.GetDevice(), m_DebugLineIndexBuffer[FrameIndex], &MemoryRequirements);

    VkMemoryAllocateInfo MemoryAllocateInfo = {};
    MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
    MemoryAllocateInfo.memoryTypeIndex = g_Context.FindMemoryType(MemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT);

    VK_CHECK(vkAllocateMemory(g_Context.GetDevice(), &MemoryAllocateInfo, nullptr, &m_DebugLineIndexBufferDeviceMemory[FrameIndex]));
    VK_CHECK(vkBindBufferMemory(g_Context.GetDevice(), m_DebugLineIndexBuffer[FrameIndex], m_DebugLineIndexBufferDeviceMemory[FrameIndex], 0));
    VK_CHECK(vkMapMemory(g_Context.GetDevice(), m_DebugLineIndexBufferDeviceMemory[FrameIndex], 0, Size, 0, (void **)&m_DebugLineIndex[FrameIndex]));
  }
}

void NpRenderer::UpdateDefaultObjectDescriptorSet() { /* TODO */ }
void NpRenderer::UpdateDebugLineDescriptorSet() {
  for (uint32_t FrameIndex = 0; FrameIndex < m_FramesInFlight; FrameIndex++) {
    std::array<VkDescriptorBufferInfo, 1> TimeDescriptorBufferInfos = {};

    TimeDescriptorBufferInfos[0].offset = 0;
    TimeDescriptorBufferInfos[0].buffer = m_TimeBuffer[FrameIndex];
    TimeDescriptorBufferInfos[0].range = VK_WHOLE_SIZE;

    std::array<VkDescriptorBufferInfo, 1> ScreenDescriptorBufferInfos = {};

    ScreenDescriptorBufferInfos[0].offset = 0;
    ScreenDescriptorBufferInfos[0].buffer = m_ScreenBuffer[FrameIndex];
    ScreenDescriptorBufferInfos[0].range = VK_WHOLE_SIZE;

    std::array<VkDescriptorBufferInfo, 1> CameraDescriptorBufferInfos = {};

    CameraDescriptorBufferInfos[0].offset = 0;
    CameraDescriptorBufferInfos[0].buffer = m_CameraBuffer[FrameIndex];
    CameraDescriptorBufferInfos[0].range = VK_WHOLE_SIZE;

    std::array<VkWriteDescriptorSet, 3> WriteDescriptorSets = {};

    WriteDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    WriteDescriptorSets[0].pNext = 0;
    WriteDescriptorSets[0].dstSet = m_DebugLineDescriptorSet[FrameIndex];
    WriteDescriptorSets[0].dstBinding = 0;
    WriteDescriptorSets[0].dstArrayElement = 0;
    WriteDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    WriteDescriptorSets[0].descriptorCount = TimeDescriptorBufferInfos.size();
    WriteDescriptorSets[0].pImageInfo = 0;
    WriteDescriptorSets[0].pBufferInfo = TimeDescriptorBufferInfos.data();
    WriteDescriptorSets[0].pTexelBufferView = 0;

    WriteDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    WriteDescriptorSets[1].pNext = 0;
    WriteDescriptorSets[1].dstSet = m_DebugLineDescriptorSet[FrameIndex];
    WriteDescriptorSets[1].dstBinding = 1;
    WriteDescriptorSets[1].dstArrayElement = 0;
    WriteDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    WriteDescriptorSets[1].descriptorCount = ScreenDescriptorBufferInfos.size();
    WriteDescriptorSets[1].pImageInfo = 0;
    WriteDescriptorSets[1].pBufferInfo = ScreenDescriptorBufferInfos.data();
    WriteDescriptorSets[1].pTexelBufferView = 0;

    WriteDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    WriteDescriptorSets[2].pNext = 0;
    WriteDescriptorSets[2].dstSet = m_DebugLineDescriptorSet[FrameIndex];
    WriteDescriptorSets[2].dstBinding = 2;
    WriteDescriptorSets[2].dstArrayElement = 0;
    WriteDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    WriteDescriptorSets[2].descriptorCount = CameraDescriptorBufferInfos.size();
    WriteDescriptorSets[2].pImageInfo = 0;
    WriteDescriptorSets[2].pBufferInfo = CameraDescriptorBufferInfos.data();
    WriteDescriptorSets[2].pTexelBufferView = 0;

    vkUpdateDescriptorSets(g_Context.GetDevice(), WriteDescriptorSets.size(), WriteDescriptorSets.data(), 0, nullptr);
  }
}

void NpRenderer::UpdateUniformBuffer(NpTransform *Transform, NpCamera *Camera) {
  m_Time[m_FrameIndex]->Time = g_Context.GetTime();
  m_Time[m_FrameIndex]->DeltaTime = g_Context.GetDeltaTime();

  m_Screen[m_FrameIndex]->Width = (float)g_Context.GetSurfaceWidth();
  m_Screen[m_FrameIndex]->Height = (float)g_Context.GetSurfaceHeight();

  glm::fvec3 Eye = Transform->GetWorldPosition();
  glm::fvec3 Center = Transform->GetWorldPosition() + Transform->GetLocalFront();
  glm::fvec3 Up = g_WorldDown;

  float Fov = glm::radians(Camera->GetFov());
  float AspectRatio = (float)g_Context.GetSurfaceWidth() / (float)g_Context.GetSurfaceHeight();
  float NearZ = Camera->GetNearZ();
  float FarZ = Camera->GetFarZ();

  glm::fmat4 View = glm::lookAt(Eye, Center, Up);
  glm::fmat4 Projection = glm::perspective(Fov, AspectRatio, NearZ, FarZ);
  glm::fmat4 ViewProjection = View * Projection;
  glm::fmat4 ViewProjectionInv = glm::inverse(ViewProjection);

  m_Camera[m_FrameIndex]->WorldPosition = Transform->GetWorldPosition();
  m_Camera[m_FrameIndex]->View = View;
  m_Camera[m_FrameIndex]->Projection = Projection;
  m_Camera[m_FrameIndex]->ViewProjection = ViewProjection;
  m_Camera[m_FrameIndex]->ViewProjectionInv = ViewProjectionInv;
}

void NpRenderer::RecordGraphicsCommand() {
  VkClearValue ColorClearValue = {};
  ColorClearValue.color.float32[0] = 0.0F;
  ColorClearValue.color.float32[1] = 0.0F;
  ColorClearValue.color.float32[2] = 0.0F;
  ColorClearValue.color.float32[3] = 1.0F;

  VkClearValue DepthClearValue = {};
  DepthClearValue.depthStencil.depth = 1.0F;
  DepthClearValue.depthStencil.stencil = 0;

  std::array<VkClearValue, 2> ClearValue = {ColorClearValue, DepthClearValue};

  VkRenderPassBeginInfo RenderPassCreateInfo = {};
  RenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  RenderPassCreateInfo.renderPass = g_Swapchain.GetRenderPass();
  RenderPassCreateInfo.framebuffer = g_Swapchain.GetFrameBuffer()[m_ImageIndex];
  RenderPassCreateInfo.renderArea.offset.x = 0;
  RenderPassCreateInfo.renderArea.offset.y = 0;
  RenderPassCreateInfo.renderArea.extent.width = g_Context.GetSurfaceWidth();
  RenderPassCreateInfo.renderArea.extent.height = g_Context.GetSurfaceHeight();
  RenderPassCreateInfo.pClearValues = ClearValue.data();
  RenderPassCreateInfo.clearValueCount = ClearValue.size();

  vkCmdBeginRenderPass(m_GraphicsCommandBuffer[m_FrameIndex], &RenderPassCreateInfo, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport Viewport = {};
  Viewport.x = 0.0F;
  Viewport.y = 0.0F;
  Viewport.width = (float)g_Context.GetSurfaceWidth();
  Viewport.height = (float)g_Context.GetSurfaceHeight();
  Viewport.minDepth = 0.0F;
  Viewport.maxDepth = 1.0F;

  vkCmdSetViewport(m_GraphicsCommandBuffer[m_FrameIndex], 0, 1, &Viewport);

  VkRect2D Scissor = {0};
  Scissor.offset.x = 0;
  Scissor.offset.y = 0;
  Scissor.extent.width = g_Context.GetSurfaceWidth();
  Scissor.extent.height = g_Context.GetSurfaceHeight();

  vkCmdSetScissor(m_GraphicsCommandBuffer[m_FrameIndex], 0, 1, &Scissor);

  /*
  {
    std::array<VkBuffer, 1> VertexBuffer = {m_DefaultObjectVertexBuffer[m_FrameIndex]};
    std::array<uint64_t, 1> VertexOffset = {0};

    vkCmdBindPipeline(m_GraphicsCommandBuffer[m_FrameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, m_DefaultObjectPipeline);
    vkCmdBindVertexBuffers(m_GraphicsCommandBuffer[m_FrameIndex], 0, VertexBuffer.size(), VertexBuffer.data(), VertexOffset.data());
    vkCmdBindIndexBuffer(m_GraphicsCommandBuffer[m_FrameIndex], m_DefaultObjectIndexBuffer[m_FrameIndex], 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(m_GraphicsCommandBuffer[m_FrameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, m_DefaultObjectPipelineLayout, 0, 1, &m_DefaultObjectDescriptorSet[m_FrameIndex], 0, nullptr);
    vkCmdDrawIndexed(m_GraphicsCommandBuffer[m_FrameIndex], 666, 1, 0, 0, 0); // TODO
  }
  */

  {
    if (m_EnableDebug) {
      std::array<VkBuffer, 1> VertexBuffer = {m_DebugLineVertexBuffer[m_FrameIndex]};
      std::array<uint64_t, 1> VertexOffset = {0};

      vkCmdBindPipeline(m_GraphicsCommandBuffer[m_FrameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, m_DebugLinePipeline);
      vkCmdBindVertexBuffers(m_GraphicsCommandBuffer[m_FrameIndex], 0, VertexBuffer.size(), VertexBuffer.data(), VertexOffset.data());
      vkCmdBindIndexBuffer(m_GraphicsCommandBuffer[m_FrameIndex], m_DebugLineIndexBuffer[m_FrameIndex], 0, VK_INDEX_TYPE_UINT32);
      vkCmdBindDescriptorSets(m_GraphicsCommandBuffer[m_FrameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, m_DebugLinePipelineLayout, 0, 1, &m_DebugLineDescriptorSet[m_FrameIndex], 0, nullptr);
      vkCmdDrawIndexed(m_GraphicsCommandBuffer[m_FrameIndex], m_DebugLineIndexOffset[m_FrameIndex], 1, 0, 0, 0);

      m_DebugLineVertexOffset[m_FrameIndex] = 0;
      m_DebugLineIndexOffset[m_FrameIndex] = 0;
    }
  }

  // IMGUI_DRAW(m_GraphicsCommandBuffer[m_FrameIndex]); // TODO

  vkCmdEndRenderPass(m_GraphicsCommandBuffer[m_FrameIndex]);
}

void NpRenderer::DestroyCommandBuffer() {
  vkFreeCommandBuffers(g_Context.GetDevice(), g_Context.GetCommandPool(), m_FramesInFlight, m_GraphicsCommandBuffer);

  delete[] m_GraphicsCommandBuffer;
}
void NpRenderer::DestroySyncObject() {
  for (uint32_t FrameIndex = 0; FrameIndex < m_FramesInFlight; FrameIndex++) {
    vkDestroySemaphore(g_Context.GetDevice(), m_GraphicsCompleteSemaphore[FrameIndex], nullptr);
    vkDestroySemaphore(g_Context.GetDevice(), m_PresentCompleteSemaphore[FrameIndex], nullptr);

    vkDestroyFence(g_Context.GetDevice(), m_FrameFence[FrameIndex], nullptr);
  }

  delete[] m_GraphicsCompleteSemaphore;
  delete[] m_PresentCompleteSemaphore;
  delete[] m_FrameFence;
}
void NpRenderer::DestroyDescriptorPool() {
  vkDestroyDescriptorPool(g_Context.GetDevice(), m_DefaultObjectDescriptorPool, nullptr);
  vkDestroyDescriptorPool(g_Context.GetDevice(), m_DebugLineDescriptorPool, nullptr);
}
void NpRenderer::DestroyDescriptorSetLayout() {
  vkDestroyDescriptorSetLayout(g_Context.GetDevice(), m_DefaultObjectDescriptorSetLayout, nullptr);
  vkDestroyDescriptorSetLayout(g_Context.GetDevice(), m_DebugLineDescriptorSetLayout, nullptr);
}
void NpRenderer::DestroyDescriptorSet() {
  delete[] m_DefaultObjectDescriptorSet;
  delete[] m_DebugLineDescriptorSet;
}
void NpRenderer::DestroyPipelineLayout() {
  vkDestroyPipelineLayout(g_Context.GetDevice(), m_DefaultObjectPipelineLayout, nullptr);
  vkDestroyPipelineLayout(g_Context.GetDevice(), m_DebugLinePipelineLayout, nullptr);
}
void NpRenderer::DestroyPipeline() {
  vkDestroyPipeline(g_Context.GetDevice(), m_DefaultObjectPipeline, nullptr);
  vkDestroyPipeline(g_Context.GetDevice(), m_DebugLinePipeline, nullptr);
}

void NpRenderer::DestroyTimeBuffer() {
  for (uint32_t FrameIndex = 0; FrameIndex < m_FramesInFlight; FrameIndex++) {
    vkUnmapMemory(g_Context.GetDevice(), m_TimeBufferDeviceMemory[FrameIndex]);
    vkFreeMemory(g_Context.GetDevice(), m_TimeBufferDeviceMemory[FrameIndex], nullptr);
    vkDestroyBuffer(g_Context.GetDevice(), m_TimeBuffer[FrameIndex], nullptr);
  }

  delete[] m_TimeBuffer;
  delete[] m_TimeBufferDeviceMemory;
  delete[] m_Time;
}
void NpRenderer::DestroyScreenBuffer() {
  for (uint32_t FrameIndex = 0; FrameIndex < m_FramesInFlight; FrameIndex++) {
    vkUnmapMemory(g_Context.GetDevice(), m_ScreenBufferDeviceMemory[FrameIndex]);
    vkFreeMemory(g_Context.GetDevice(), m_ScreenBufferDeviceMemory[FrameIndex], nullptr);
    vkDestroyBuffer(g_Context.GetDevice(), m_ScreenBuffer[FrameIndex], nullptr);
  }

  delete[] m_ScreenBuffer;
  delete[] m_ScreenBufferDeviceMemory;
  delete[] m_Screen;
}
void NpRenderer::DestroyCameraBuffer() {
  for (uint32_t FrameIndex = 0; FrameIndex < m_FramesInFlight; FrameIndex++) {
    vkUnmapMemory(g_Context.GetDevice(), m_CameraBufferDeviceMemory[FrameIndex]);
    vkFreeMemory(g_Context.GetDevice(), m_CameraBufferDeviceMemory[FrameIndex], nullptr);
    vkDestroyBuffer(g_Context.GetDevice(), m_CameraBuffer[FrameIndex], nullptr);
  }

  delete[] m_CameraBuffer;
  delete[] m_CameraBufferDeviceMemory;
  delete[] m_Camera;
}
void NpRenderer::DestroyDefaultObjectVertexBuffer() { /* TODO */ }
void NpRenderer::DestroyDefaultObjectIndexBuffer() { /* TODO */ }
void NpRenderer::DestroyDebugLineVertexBuffer() {
  for (uint32_t FrameIndex = 0; FrameIndex < m_FramesInFlight; FrameIndex++) {
    vkUnmapMemory(g_Context.GetDevice(), m_DebugLineVertexBufferDeviceMemory[FrameIndex]);
    vkFreeMemory(g_Context.GetDevice(), m_DebugLineVertexBufferDeviceMemory[FrameIndex], nullptr);
    vkDestroyBuffer(g_Context.GetDevice(), m_DebugLineVertexBuffer[FrameIndex], nullptr);
  }

  delete[] m_DebugLineVertexBuffer;
  delete[] m_DebugLineVertexBufferDeviceMemory;
  delete[] m_DebugLineVertex;
}
void NpRenderer::DestroyDebugLineIndexBuffer() {
  for (uint32_t FrameIndex = 0; FrameIndex < m_FramesInFlight; FrameIndex++) {
    vkUnmapMemory(g_Context.GetDevice(), m_DebugLineIndexBufferDeviceMemory[FrameIndex]);
    vkFreeMemory(g_Context.GetDevice(), m_DebugLineIndexBufferDeviceMemory[FrameIndex], nullptr);
    vkDestroyBuffer(g_Context.GetDevice(), m_DebugLineIndexBuffer[FrameIndex], nullptr);
  }

  delete[] m_DebugLineIndexBuffer;
  delete[] m_DebugLineIndexBufferDeviceMemory;
  delete[] m_DebugLineIndex;
}

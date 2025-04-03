#include <np_editor_pch.hpp>
#include <np_editor_context.hpp>
#include <np_editor_imgui.hpp>
#include <np_editor_swapchain.hpp>

NpImGui::NpImGui() {
}
NpImGui::~NpImGui() {
}

void NpImGui::Create() {
  VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo = {};
  DescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  DescriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  DescriptorPoolCreateInfo.maxSets = 0;

  std::vector<VkDescriptorPoolSize> const DescriptorPoolSizes = {
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE},
  };

  for (VkDescriptorPoolSize const &DescriptorPoolSize : DescriptorPoolSizes) {
    DescriptorPoolCreateInfo.maxSets += DescriptorPoolSize.descriptorCount;
  }

  DescriptorPoolCreateInfo.pPoolSizes = DescriptorPoolSizes.data();
  DescriptorPoolCreateInfo.poolSizeCount = DescriptorPoolSizes.size();

  vkCreateDescriptorPool(g_Context.GetDevice(), &DescriptorPoolCreateInfo, nullptr, &m_DescriptorPool);

  IMGUI_CHECKVERSION();

  m_Context = ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigWindowsMoveFromTitleBarOnly = 1;

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForVulkan(g_Context.GetWindow(), true);

  ImGui_ImplVulkan_InitInfo ImguiVulkanInitInfo = {};
  ImguiVulkanInitInfo.Instance = g_Context.GetInstance();
  ImguiVulkanInitInfo.PhysicalDevice = g_Context.GetPhysicalDevice();
  ImguiVulkanInitInfo.Device = g_Context.GetDevice();
  ImguiVulkanInitInfo.QueueFamily = g_Context.GetGraphicsQueueIndex();
  ImguiVulkanInitInfo.Queue = g_Context.GetGraphicsQueue();
  ImguiVulkanInitInfo.PipelineCache = nullptr;
  ImguiVulkanInitInfo.DescriptorPool = m_DescriptorPool;
  ImguiVulkanInitInfo.RenderPass = g_Swapchain.GetRenderPass();
  ImguiVulkanInitInfo.Subpass = 0;
  ImguiVulkanInitInfo.MinImageCount = g_Swapchain.GetImageCount();
  ImguiVulkanInitInfo.ImageCount = g_Swapchain.GetImageCount();
  ImguiVulkanInitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  ImguiVulkanInitInfo.Allocator = nullptr;
  ImguiVulkanInitInfo.CheckVkResultFn = nullptr;

  ImGui_ImplVulkan_Init(&ImguiVulkanInitInfo);
}
void NpImGui::Draw(VkCommandBuffer CommandBuffer) {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();

  ImGui::NewFrame();
  ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

  ImGui::Begin("Hello World");
  ImGui::End();

  ImGui::Render();

  ImDrawData *DrawData = ImGui::GetDrawData();

  ImGui_ImplVulkan_RenderDrawData(DrawData, CommandBuffer);
}
void NpImGui::Destroy() {
  ImGui_ImplVulkan_DestroyFontsTexture();
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();

  ImGui::DestroyContext(m_Context);

  vkDestroyDescriptorPool(g_Context.GetDevice(), m_DescriptorPool, nullptr);
}

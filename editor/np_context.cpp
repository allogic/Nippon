#include <np_pch.hpp>
#include <np_context.hpp>
#include <np_macros.hpp>

// #include <engine/imgui.h>
// #include <engine/macros.h>
// #include <engine/swapchain.h>

#if defined(BUILD_DEBUG)
static VkBool32 VulkanDebugMessageProc(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity, VkDebugUtilsMessageTypeFlagsEXT MessageType, VkDebugUtilsMessengerCallbackDataEXT const *CallbackData, void *UserData);
#endif

NpContext::NpContext() {}
NpContext::~NpContext() {}

bool NpContext::Create(int32_t Width, int32_t Height) {
  if (glfwInit()) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_Window = glfwCreateWindow(Width, Height, "Editor", nullptr, nullptr);

    if (m_Window) {
      CreateInstance();
      CreateSurface();

      /*
      context_find_physical_device();
      context_find_physical_device_queue_families();

      context_check_physical_device_extensions();

      context_create_device();

      context_check_surface_capabilities();

      context_resize_surface();

      context_create_command_pool();

      swapchain_create();
      renderer_create();
      */

      return true;
    } else {
      std::printf("Creating GLFW window failed!\n");
    }
  } else {
    std::printf("Initializing GLFW context failed!\n");
  }

  return false;
}
bool NpContext::IsRunning() { return !glfwWindowShouldClose(m_Window); }
void NpContext::BeginFrame() {
  if (m_SwapchainIsDirty) {
    m_SwapchainIsDirty = false;

    // renderer_destroy();
    // swapchain_destroy();

    ResizeSurface();

    // swapchain_create();
    // renderer_create();
  }

  if (m_RendererIsDirty) {
    m_RendererIsDirty = false;

    // renderer_destroy();

    // renderer_create();
  }
}
void NpContext::EndFrame() {}
void NpContext::Destroy() {
  // renderer_destroy();

  // swapchain_destroy();

  DestroyCommandPool();
  DestroyDevice();
  DestroySurface();
  DestroyInstance();
}

void NpContext::CreateInstance() {
  VkApplicationInfo ApplicationInfo = {};
  ApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  ApplicationInfo.pApplicationName = "";
  ApplicationInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
  ApplicationInfo.pEngineName = "";
  ApplicationInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
  ApplicationInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo InstanceCreateInfo = {};
  InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  InstanceCreateInfo.pApplicationInfo = &ApplicationInfo;

  uint32_t GlfwExtensionCount = 0;
  const char **GlfwExtensions = glfwGetRequiredInstanceExtensions(&GlfwExtensionCount);

  std::vector<char const *> LayerExtensions(GlfwExtensionCount + m_LayerExtensions.size());

  for (uint32_t ExtensionIndex = 0; ExtensionIndex < GlfwExtensionCount; ExtensionIndex++) {
    LayerExtensions[ExtensionIndex] = GlfwExtensions[ExtensionIndex];
  }

  for (uint32_t ExtensionIndex = 0; ExtensionIndex < m_LayerExtensions.size(); ExtensionIndex++) {
    LayerExtensions[ExtensionIndex + GlfwExtensionCount] = m_LayerExtensions[ExtensionIndex];
  }

  InstanceCreateInfo.enabledExtensionCount = LayerExtensions.size();
  InstanceCreateInfo.ppEnabledExtensionNames = LayerExtensions.data();

#if defined(BUILD_DEBUG)
  VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfo = {};
  DebugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  DebugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  DebugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  DebugUtilsMessengerCreateInfo.pfnUserCallback = VulkanDebugMessageProc;

  InstanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&DebugUtilsMessengerCreateInfo;
  InstanceCreateInfo.enabledLayerCount = s_ValidationLayers.size();
  InstanceCreateInfo.ppEnabledLayerNames = s_ValidationLayers.data();
#endif

  VK_CHECK(vkCreateInstance(&InstanceCreateInfo, nullptr, &m_Instance));

#if defined(BUILD_DEBUG)
  m_CreateDebugUtilsMessengerExt = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
  m_DestroyDebugUtilsMessengerExt = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");

  VK_CHECK(m_CreateDebugUtilsMessengerExt(m_Instance, &DebugUtilsMessengerCreateInfo, nullptr, &m_DebugMessenger));
#endif
}
void NpContext::CreateSurface() { VK_CHECK(glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface)); }
void NpContext::CreateDevice() {
  std::array<VkDeviceQueueCreateInfo, 2> DeviceQueueCreateInfos = {};

  float QueuePriority = 1.0F;

  DeviceQueueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  DeviceQueueCreateInfos[0].queueFamilyIndex = m_GraphicsQueueIndex;
  DeviceQueueCreateInfos[0].queueCount = 1;
  DeviceQueueCreateInfos[0].pQueuePriorities = &QueuePriority;

  DeviceQueueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  DeviceQueueCreateInfos[1].queueFamilyIndex = m_PresentQueueIndex;
  DeviceQueueCreateInfos[1].queueCount = 1;
  DeviceQueueCreateInfos[1].pQueuePriorities = &QueuePriority;

  VkPhysicalDeviceDescriptorIndexingFeatures PhysicalDeviceDescriptorIndexingFeatures = {};
  PhysicalDeviceDescriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
  PhysicalDeviceDescriptorIndexingFeatures.pNext = nullptr;

  VkPhysicalDeviceFeatures2 PhysicalDeviceFeatures2 = {};
  PhysicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
  PhysicalDeviceFeatures2.pNext = &PhysicalDeviceDescriptorIndexingFeatures;

  vkGetPhysicalDeviceFeatures2(m_PhysicalDevice, &PhysicalDeviceFeatures2);

  PhysicalDeviceFeatures2.features.samplerAnisotropy = 1;

  VkDeviceCreateInfo DeviceCreateInfo = {};
  DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  DeviceCreateInfo.pQueueCreateInfos = DeviceQueueCreateInfos.data();
  DeviceCreateInfo.queueCreateInfoCount = DeviceQueueCreateInfos.size();
  DeviceCreateInfo.pEnabledFeatures = nullptr;
  DeviceCreateInfo.pNext = &PhysicalDeviceFeatures2;
  DeviceCreateInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();
  DeviceCreateInfo.enabledExtensionCount = m_DeviceExtensions.size();

#if defined(BUILD_DEBUG)
  DeviceCreateInfo.ppEnabledLayerNames = s_ValidationLayers.data();
  DeviceCreateInfo.enabledLayerCount = s_ValidationLayers.size();
#endif

  VK_CHECK(vkCreateDevice(m_PhysicalDevice, &DeviceCreateInfo, nullptr, &m_Device));

  vkGetDeviceQueue(m_Device, m_GraphicsQueueIndex, 0, &m_GraphicsQueue);
  vkGetDeviceQueue(m_Device, m_PresentQueueIndex, 0, &m_PresentQueue);
}
void NpContext::CreateCommandPool() {
  VkCommandPoolCreateInfo CommandPoolCreateInfo = {};
  CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  CommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  CommandPoolCreateInfo.queueFamilyIndex = m_GraphicsQueueIndex;

  VK_CHECK(vkCreateCommandPool(m_Device, &CommandPoolCreateInfo, nullptr, &m_CommandPool));
}

void NpContext::DestroyInstance() {
#if defined(BUILD_DEBUG)
  m_DestroyDebugUtilsMessengerExt(m_Instance, m_DebugMessenger, nullptr);
#endif

  vkDestroyInstance(m_Instance, nullptr);
}
void NpContext::DestroySurface() { vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr); }
void NpContext::DestroyDevice() { vkDestroyDevice(m_Device, nullptr); }
void NpContext::DestroyCommandPool() { vkDestroyCommandPool(m_Device, m_CommandPool, nullptr); }

void NpContext::CheckSurfaceCapabilities() {
  uint32_t SurfaceFormatCount = 0;
  VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &SurfaceFormatCount, nullptr));

  std::unique_ptr<VkSurfaceFormatKHR[]> SurfaceFormats = std::make_unique<VkSurfaceFormatKHR[]>(SurfaceFormatCount);
  VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &SurfaceFormatCount, SurfaceFormats.get()));

  uint32_t PresentModeCount = 0;
  VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &PresentModeCount, nullptr));

  std::unique_ptr<VkPresentModeKHR[]> PresentModes = std::make_unique<VkPresentModeKHR[]>(PresentModeCount);
  VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &PresentModeCount, PresentModes.get()));

  for (uint64_t SurfaceFormatIndex = 0; SurfaceFormatIndex < SurfaceFormatCount; SurfaceFormatIndex++) {
    VkSurfaceFormatKHR SurfaceFormat = SurfaceFormats[SurfaceFormatIndex];

    if ((SurfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM) && (SurfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)) {
      m_PreferedSurfaceFormat = SurfaceFormat;

      break;
    }

    SurfaceFormatIndex++;
  }

  for (uint64_t PresentModeIndex = 0; PresentModeIndex < PresentModeCount; PresentModeIndex++) {
    VkPresentModeKHR PresentMode = PresentModes[PresentModeIndex];

    if (PresentModeIndex == VK_PRESENT_MODE_MAILBOX_KHR) {
      m_PreferedPresentMode = PresentMode;

      break;
    }

    PresentModeIndex++;
  }
}
void NpContext::CheckPhysicalDeviceExtensions() {
  uint32_t AvailableDeviceExtensionCount = 0;
  VK_CHECK(vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &AvailableDeviceExtensionCount, nullptr));

  std::unique_ptr<VkExtensionProperties[]> AvailableDeviceExtensions = std::make_unique<VkExtensionProperties[]>(AvailableDeviceExtensionCount);
  VK_CHECK(vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &AvailableDeviceExtensionCount, AvailableDeviceExtensions.get()));

  printf("Device Extensions\n");

  for (uint64_t DeviceExtensionIndex = 0; DeviceExtensionIndex < m_DeviceExtensions.size(); DeviceExtensionIndex++) {
    bool DeviceExtensionAvailable = false;

    for (uint64_t AvailableDeviceExtensionIndex = 0; AvailableDeviceExtensionIndex < AvailableDeviceExtensionCount; AvailableDeviceExtensionIndex++) {
      VkExtensionProperties ExtensionProperties = AvailableDeviceExtensions[AvailableDeviceExtensionIndex];

      if (strcmp(m_DeviceExtensions[DeviceExtensionIndex], ExtensionProperties.extensionName) == 0) {
        std::printf("\tFound %s\n", m_DeviceExtensions[DeviceExtensionIndex]);

        DeviceExtensionAvailable = true;

        break;
      }
    }

    if (!DeviceExtensionAvailable) {
      std::printf("\tMissing %s\n", m_DeviceExtensions[DeviceExtensionIndex]);

      break;
    }
  }

  std::printf("\n");
}

void NpContext::ResizeSurface() {
  VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &m_SurfaceCapabilities));

  m_SurfaceWidth = m_SurfaceCapabilities.currentExtent.width;
  m_SurfaceHeight = m_SurfaceCapabilities.currentExtent.height;
}

void NpContext::FindPhysicalDevice() {
  uint32_t PhysicalDeviceCount = 0;
  VK_CHECK(vkEnumeratePhysicalDevices(m_Instance, &PhysicalDeviceCount, nullptr));

  std::unique_ptr<VkPhysicalDevice[]> PhysicalDevices = std::make_unique<VkPhysicalDevice[]>(PhysicalDeviceCount);
  VK_CHECK(vkEnumeratePhysicalDevices(m_Instance, &PhysicalDeviceCount, PhysicalDevices.get()));

  for (uint64_t PhysicalDeviceIndex = 0; PhysicalDeviceIndex < PhysicalDeviceCount; PhysicalDeviceIndex++) {
    VkPhysicalDevice PhysicalDevice = PhysicalDevices[PhysicalDeviceIndex];

    vkGetPhysicalDeviceProperties(PhysicalDevice, &m_PhysicalDeviceProperties);
    vkGetPhysicalDeviceFeatures(PhysicalDevice, &m_PhysicalDeviceFeatures);
    vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &m_PhysicalDeviceMemoryProperties);

    if (m_PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      if (m_PhysicalDeviceFeatures.geometryShader && m_PhysicalDeviceFeatures.samplerAnisotropy) {
        m_PhysicalDevice = PhysicalDevice;

        break;
      }
    }
  }
}
void NpContext::FindPhysicalDeviceQueueFamilies() {
  uint32_t QueueFamilyPropertyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &QueueFamilyPropertyCount, nullptr);

  std::unique_ptr<VkQueueFamilyProperties[]> QueueFamilyProperties = std::make_unique<VkQueueFamilyProperties[]>(QueueFamilyPropertyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &QueueFamilyPropertyCount, QueueFamilyProperties.get());

  for (uint64_t PhysicalDeviceQueueFamilyPropertyIndex = 0; PhysicalDeviceQueueFamilyPropertyIndex < QueueFamilyPropertyCount; PhysicalDeviceQueueFamilyPropertyIndex++) {
    VkQueueFamilyProperties QueueProperties = QueueFamilyProperties[PhysicalDeviceQueueFamilyPropertyIndex];

    uint32_t GraphicsSupport = 0;
    uint32_t ComputeSupport = 0;
    uint32_t PresentSupport = 0;

    GraphicsSupport = QueueProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
    ComputeSupport = QueueProperties.queueFlags & VK_QUEUE_COMPUTE_BIT;

    VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, (uint32_t)PhysicalDeviceQueueFamilyPropertyIndex, m_Surface, &PresentSupport));

    if (GraphicsSupport && (m_GraphicsQueueIndex == -1)) {
      m_GraphicsQueueIndex = (uint32_t)PhysicalDeviceQueueFamilyPropertyIndex;
    } else if (PresentSupport && (m_PresentQueueIndex == -1)) {
      m_PresentQueueIndex = (uint32_t)PhysicalDeviceQueueFamilyPropertyIndex;
    }

    if ((m_GraphicsQueueIndex != -1) && (m_PresentQueueIndex != -1)) {
      break;
    }
  }

  printf("Queue Indices\n");
  printf("\tGraphics Queue Index %d\n", m_GraphicsQueueIndex);
  printf("\tPresent Queue Index %d\n", m_PresentQueueIndex);
  printf("\n");
}

#if defined(BUILD_DEBUG)
static VkBool32 VulkanDebugMessageProc(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity, VkDebugUtilsMessageTypeFlagsEXT MessageType, VkDebugUtilsMessengerCallbackDataEXT const *CallbackData, void *UserData) {
  std::printf("%s\n", CallbackData->pMessage);

  return 0;
}
#endif

/*
int32_t context_find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags memory_property_flags) {
  int32_t memory_type = -1;

  uint32_t memory_type_index = 0;
  while (memory_type_index < g_context_physical_device_memory_properties.memoryTypeCount) {
    if ((type_filter & (1 << memory_type_index)) && ((g_context_physical_device_memory_properties.memoryTypes[memory_type_index].propertyFlags & memory_property_flags) == memory_property_flags)) {
      memory_type = (int32_t)memory_type_index;

      break;
    }

    memory_type_index++;
  }

  return memory_type;
}

VkCommandBuffer context_begin_command_buffer(void) {
  VkCommandBuffer command_buffer = 0;

  VkCommandBufferAllocateInfo command_buffer_allocate_info = {0};
  command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_allocate_info.commandPool = g_context_command_pool;
  command_buffer_allocate_info.commandBufferCount = 1;

  VK_CHECK(vkAllocateCommandBuffers(g_context_device, &command_buffer_allocate_info, &command_buffer));

  VkCommandBufferBeginInfo command_buffer_begin_info = {0};
  command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  VK_CHECK(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));

  return command_buffer;
}
void context_end_command_buffer(VkCommandBuffer command_buffer) {
  VK_CHECK(vkEndCommandBuffer(command_buffer));

  VkSubmitInfo submit_info = {0};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffer;

  VK_CHECK(vkQueueSubmit(g_context_graphics_queue, 1, &submit_info, 0));
  VK_CHECK(vkQueueWaitIdle(g_context_graphics_queue));

  vkFreeCommandBuffers(g_context_device, g_context_command_pool, 1, &command_buffer);
}
*/

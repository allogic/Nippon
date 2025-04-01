#include <np_editor_pch.hpp>
#include <np_editor_context.hpp>
#include <np_editor_macro.hpp>
#include <np_editor_renderer.hpp>
#include <np_editor_swapchain.hpp>

NpContext g_Context = {};

static void GlfwWindowPosition(GLFWwindow *Window, int X, int Y);
static void GlfwWindowSize(GLFWwindow *Window, int Width, int Height);
static void GlfwWindowClose(GLFWwindow *Window);
static void GlfwWindowRefresh(GLFWwindow *Window);
static void GlfwWindowFocus(GLFWwindow *Window, int Focused);
static void GlfwWindowIconify(GLFWwindow *Window, int Iconified);
static void GlfwWindowMaximize(GLFWwindow *Window, int Maximized);
static void GlfwWindowContentScale(GLFWwindow *Window, float X, float Y);

static void GlfwFrameBufferSize(GLFWwindow *Window, int Width, int Height);

static void GlfwKey(GLFWwindow *window, int Key, int ScanCode, int Action, int Mods);
static void GlfwChar(GLFWwindow *Window, unsigned int CodePoint);
static void GlfwCharMods(GLFWwindow *window, unsigned int codepoint, int mods);
static void GlfwMouseButton(GLFWwindow *Window, int Button, int Action, int Mods);
static void GlfwCursorPosition(GLFWwindow *Window, double X, double Y);
static void GlfwCursorEnter(GLFWwindow *Window, int Entered);
static void GlfwScroll(GLFWwindow *Window, double X, double Y);

static void GlfwJoystick(int Jid, int Event);

static void GlfwMonitor(GLFWmonitor *Monitor, int Event);

#if defined(BUILD_DEBUG)
static VkBool32
VulkanDebugMessageProc(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity, VkDebugUtilsMessageTypeFlagsEXT MessageType, VkDebugUtilsMessengerCallbackDataEXT const *CallbackData, void *UserData);
#endif

NpContext::NpContext() {}
NpContext::~NpContext() {}

bool NpContext::Create(int32_t Width, int32_t Height) {
  if (glfwInit()) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_Window = glfwCreateWindow(Width, Height, "Editor " NP_VERSION_MAJOR "." NP_VERSION_MINOR "." NP_VERSION_PATCH " (" GIT_VERSION_HASH ")", nullptr, nullptr);

    if (m_Window) {
      glfwSetWindowPosCallback(m_Window, GlfwWindowPosition);
      glfwSetWindowSizeCallback(m_Window, GlfwWindowSize);
      glfwSetWindowCloseCallback(m_Window, GlfwWindowClose);
      glfwSetWindowRefreshCallback(m_Window, GlfwWindowRefresh);
      glfwSetWindowFocusCallback(m_Window, GlfwWindowFocus);
      glfwSetWindowIconifyCallback(m_Window, GlfwWindowIconify);
      glfwSetWindowMaximizeCallback(m_Window, GlfwWindowMaximize);
      glfwSetWindowContentScaleCallback(m_Window, GlfwWindowContentScale);

      glfwSetFramebufferSizeCallback(m_Window, GlfwFrameBufferSize);

      glfwSetKeyCallback(m_Window, GlfwKey);
      glfwSetCharCallback(m_Window, GlfwChar);
      glfwSetCharModsCallback(m_Window, GlfwCharMods);
      glfwSetMouseButtonCallback(m_Window, GlfwMouseButton);
      glfwSetCursorPosCallback(m_Window, GlfwCursorPosition);
      glfwSetCursorEnterCallback(m_Window, GlfwCursorEnter);
      glfwSetScrollCallback(m_Window, GlfwScroll);

      glfwSetJoystickCallback(GlfwJoystick);

      glfwSetMonitorCallback(GlfwMonitor);

      CreateInstance();
      CreateSurface();

      FindPhysicalDevice();
      FindPhysicalDeviceQueueFamilies();

      CheckPhysicalDeviceExtensions();

      CreateDevice();

      CheckSurfaceCapabilities();

      ResizeSurface();

      CreateCommandPool();

      g_Swapchain.Create(0);
      g_Renderer.Create(1);

      return true;
    } else {
      std::printf("Creating GLFW window failed!\n");
    }
  } else {
    std::printf("Initializing GLFW context failed!\n");
  }

  return false;
}
void NpContext::Run() {
  while (!glfwWindowShouldClose(m_Window)) {
    glfwPollEvents();

    if (m_SwapchainIsDirty) {
      m_SwapchainIsDirty = false;

      g_Renderer.Destroy();
      g_Swapchain.Destroy();

      ResizeSurface();

      g_Swapchain.Create(0);
      g_Renderer.Create(1);
    }

    if (m_RendererIsDirty) {
      m_RendererIsDirty = false;

      g_Renderer.Destroy();

      g_Renderer.Create(1);
    }

    g_Renderer.Update();
  }
}
void NpContext::Destroy() {
  g_Renderer.Destroy();
  g_Swapchain.Destroy();

  DestroyCommandPool();
  DestroyDevice();
  DestroySurface();
  DestroyInstance();
}

int32_t NpContext::FindMemoryType(uint32_t TypeFilter, VkMemoryPropertyFlags MemoryPropertyFlags) {
  for (uint32_t MemoryTypeIndex = 0; MemoryTypeIndex < m_PhysicalDeviceMemoryProperties.memoryTypeCount; MemoryTypeIndex++) {
    if ((TypeFilter & (1 << MemoryTypeIndex)) && ((m_PhysicalDeviceMemoryProperties.memoryTypes[MemoryTypeIndex].propertyFlags & MemoryPropertyFlags) == MemoryPropertyFlags)) {
      return (int32_t)MemoryTypeIndex;
    }
  }

  return -1;
}

VkCommandBuffer NpContext::BeginCommandBuffer() {
  VkCommandBuffer CommandBuffer = nullptr;

  VkCommandBufferAllocateInfo CommandBufferAllocateInfo = {};
  CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  CommandBufferAllocateInfo.commandPool = m_CommandPool;
  CommandBufferAllocateInfo.commandBufferCount = 1;

  VK_CHECK(vkAllocateCommandBuffers(m_Device, &CommandBufferAllocateInfo, &CommandBuffer));

  VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
  CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  VK_CHECK(vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo));

  return CommandBuffer;
}
void NpContext::EndCommandBuffer(VkCommandBuffer CommandBuffer) {
  VK_CHECK(vkEndCommandBuffer(CommandBuffer));

  VkSubmitInfo SubmitInfo = {};
  SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  SubmitInfo.commandBufferCount = 1;
  SubmitInfo.pCommandBuffers = &CommandBuffer;

  VK_CHECK(vkQueueSubmit(m_GraphicsQueue, 1, &SubmitInfo, nullptr));
  VK_CHECK(vkQueueWaitIdle(m_GraphicsQueue));

  vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &CommandBuffer);
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
  InstanceCreateInfo.enabledLayerCount = m_ValidationLayers.size();
  InstanceCreateInfo.ppEnabledLayerNames = m_ValidationLayers.data();
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
  float QueuePriority = 1.0F;

  std::array<VkDeviceQueueCreateInfo, 2> DeviceQueueCreateInfos = {};

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
  DeviceCreateInfo.ppEnabledLayerNames = m_ValidationLayers.data();
  DeviceCreateInfo.enabledLayerCount = m_ValidationLayers.size();
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

  VkSurfaceFormatKHR *SurfaceFormats = new VkSurfaceFormatKHR[SurfaceFormatCount];
  VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &SurfaceFormatCount, SurfaceFormats));

  uint32_t PresentModeCount = 0;
  VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &PresentModeCount, nullptr));

  VkPresentModeKHR *PresentModes = new VkPresentModeKHR[PresentModeCount];
  VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &PresentModeCount, PresentModes));

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

  delete[] SurfaceFormats;
  delete[] PresentModes;
}
void NpContext::CheckPhysicalDeviceExtensions() {
  uint32_t AvailableDeviceExtensionCount = 0;
  VK_CHECK(vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &AvailableDeviceExtensionCount, nullptr));

  VkExtensionProperties *AvailableDeviceExtensions = new VkExtensionProperties[AvailableDeviceExtensionCount];
  VK_CHECK(vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &AvailableDeviceExtensionCount, AvailableDeviceExtensions));

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

  delete[] AvailableDeviceExtensions;
}

void NpContext::ResizeSurface() {
  VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &m_SurfaceCapabilities));

  m_SurfaceWidth = m_SurfaceCapabilities.currentExtent.width;
  m_SurfaceHeight = m_SurfaceCapabilities.currentExtent.height;
}

void NpContext::FindPhysicalDevice() {
  uint32_t PhysicalDeviceCount = 0;
  VK_CHECK(vkEnumeratePhysicalDevices(m_Instance, &PhysicalDeviceCount, nullptr));

  VkPhysicalDevice *PhysicalDevices = new VkPhysicalDevice[PhysicalDeviceCount];
  VK_CHECK(vkEnumeratePhysicalDevices(m_Instance, &PhysicalDeviceCount, PhysicalDevices));

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

  delete[] PhysicalDevices;
}
void NpContext::FindPhysicalDeviceQueueFamilies() {
  uint32_t QueueFamilyPropertyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &QueueFamilyPropertyCount, nullptr);

  VkQueueFamilyProperties *QueueFamilyProperties = new VkQueueFamilyProperties[QueueFamilyPropertyCount];
  vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &QueueFamilyPropertyCount, QueueFamilyProperties);

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

  delete[] QueueFamilyProperties;
}

static void GlfwWindowPosition(GLFWwindow *Window, int X, int Y) {}
static void GlfwWindowSize(GLFWwindow *Window, int Width, int Height) {}
static void GlfwWindowClose(GLFWwindow *Window) {}
static void GlfwWindowRefresh(GLFWwindow *Window) {}
static void GlfwWindowFocus(GLFWwindow *Window, int Focused) {}
static void GlfwWindowIconify(GLFWwindow *Window, int Iconified) {}
static void GlfwWindowMaximize(GLFWwindow *Window, int Maximized) {}
static void GlfwWindowContentScale(GLFWwindow *Window, float X, float Y) {}

static void GlfwFrameBufferSize(GLFWwindow *Window, int Width, int Height) {}

static void GlfwKey(GLFWwindow *Window, int Key, int ScanCode, int Action, int Mods) {}
static void GlfwChar(GLFWwindow *Window, unsigned int CodePoint) {}
static void GlfwCharMods(GLFWwindow *window, unsigned int codepoint, int mods) {}
static void GlfwMouseButton(GLFWwindow *Window, int Button, int Action, int Mods) {}
static void GlfwCursorPosition(GLFWwindow *Window, double X, double Y) {}
static void GlfwCursorEnter(GLFWwindow *Window, int Entered) {}
static void GlfwScroll(GLFWwindow *Window, double X, double Y) {}

static void GlfwJoystick(int Jid, int Event) {}

static void GlfwMonitor(GLFWmonitor *Monitor, int Event) {}

#if defined(BUILD_DEBUG)
static VkBool32 VulkanDebugMessageProc(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity, VkDebugUtilsMessageTypeFlagsEXT MessageType, VkDebugUtilsMessengerCallbackDataEXT const *CallbackData, void *UserData) {
  std::printf("%s\n", CallbackData->pMessage);

  return 0;
}
#endif

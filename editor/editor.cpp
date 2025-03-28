#include <pch.hpp>
#include <editor.hpp>
#include <macros.hpp>

// #include <engine/imgui.h>
// #include <engine/macros.h>
// #include <engine/swapchain.h>

#if defined(BUILD_DEBUG)
static VkBool32 VulkanDebugMessageProc(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity, VkDebugUtilsMessageTypeFlagsEXT MessageType, VkDebugUtilsMessengerCallbackDataEXT const *CallbackData, void *UserData);
#endif

#if defined(BUILD_DEBUG)
static std::array<char const *, 1> s_ValidationLayers = {
    "VK_LAYER_KHRONOS_validation",
};
#endif

#if defined(BUILD_DEBUG)
static std::array<char const *, 3> s_LayerExtensions = {
    "VK_KHR_surface",
    "VK_KHR_win32_surface",
    "VK_EXT_debug_utils",
};
#else
static std::array<char const *, 2> s_LayerExtensions = {
    "VK_KHR_surface",
    "VK_KHR_win32_surface",
};
#endif

static std::array<char const *, 2> s_DeviceExtensions = {
    "VK_KHR_swapchain",
    "VK_EXT_descriptor_indexing",
};

NpEditor::NpEditor() {}
NpEditor::~NpEditor() {}

bool NpEditor::Create(int32_t Width, int32_t Height) {
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
bool NpEditor::IsRunning() { return !glfwWindowShouldClose(m_Window); }
void NpEditor::BeginFrame() {
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
void NpEditor::EndFrame() {}
void NpEditor::Destroy() {
  // renderer_destroy();

  // swapchain_destroy();

  DestroyCommandPool();
  DestroyDevice();
  DestroySurface();
  DestroyInstance();
}

void NpEditor::CreateInstance() {
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
  InstanceCreateInfo.enabledExtensionCount = s_LayerExtensions.size();
  InstanceCreateInfo.ppEnabledExtensionNames = s_LayerExtensions.data();

  // uint32_t glfwExtensionCount = 0;
  // const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  // InstanceCreateInfo.enabledExtensionCount = glfwExtensionCount;
  // InstanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;

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

  VK_CHECK(vkCreateInstance(&InstanceCreateInfo, 0, &m_Instance));

#if defined(BUILD_DEBUG)
  m_CreateDebugUtilsMessengerExt = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
  m_DestroyDebugUtilsMessengerExt = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");

  VK_CHECK(m_CreateDebugUtilsMessengerExt(m_Instance, &DebugUtilsMessengerCreateInfo, 0, &m_DebugMessenger));
#endif
}
void NpEditor::CreateSurface() { VK_CHECK(glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface)); }
void NpEditor::CreateDevice() {}
void NpEditor::CreateCommandPool() {}

void NpEditor::DestroyInstance() {}
void NpEditor::DestroySurface() {}
void NpEditor::DestroyDevice() {}
void NpEditor::DestroyCommandPool() {}

void NpEditor::CheckSurfaceCapabilities() {}
void NpEditor::CheckPhysicalDeviceExtensions() {}

void NpEditor::ResizeSurface() {}

void NpEditor::FindPhysicalDevice() {}
void NpEditor::FindPhysicalDeviceQueueFamilies() {}

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

static void context_create_surface(void) {
  VkWin32SurfaceCreateInfoKHR surface_create_info = {0};
  surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  surface_create_info.hwnd = g_context_window;
  surface_create_info.hinstance = g_context_module;

  VK_CHECK(vkCreateWin32SurfaceKHR(g_context_instance, &surface_create_info, 0, &g_context_surface));
}
static void context_create_device(void) {
  VkDeviceQueueCreateInfo device_queue_create_infos[2] = {0};

  float queue_priority = 1.0F;

  device_queue_create_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  device_queue_create_infos[0].queueFamilyIndex = g_context_graphics_queue_index;
  device_queue_create_infos[0].queueCount = 1;
  device_queue_create_infos[0].pQueuePriorities = &queue_priority;

  device_queue_create_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  device_queue_create_infos[1].queueFamilyIndex = g_context_present_queue_index;
  device_queue_create_infos[1].queueCount = 1;
  device_queue_create_infos[1].pQueuePriorities = &queue_priority;

  VkPhysicalDeviceDescriptorIndexingFeatures physical_device_descriptor_indexing_features = {0};
  physical_device_descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
  physical_device_descriptor_indexing_features.pNext = 0;

  VkPhysicalDeviceFeatures2 physical_device_features_2 = {0};
  physical_device_features_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
  physical_device_features_2.pNext = &physical_device_descriptor_indexing_features;

  vkGetPhysicalDeviceFeatures2(g_context_physical_device, &physical_device_features_2);

  physical_device_features_2.features.samplerAnisotropy = 1;
  physical_device_features_2.features.shaderFloat64 = 1;

  VkDeviceCreateInfo device_create_info = {0};
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.pQueueCreateInfos = device_queue_create_infos;
  device_create_info.queueCreateInfoCount = CORE_ARRAY_COUNT(device_queue_create_infos);
  device_create_info.pEnabledFeatures = 0;
  device_create_info.pNext = &physical_device_features_2;
  device_create_info.ppEnabledExtensionNames = s_context_device_extensions;
  device_create_info.enabledExtensionCount = CORE_ARRAY_COUNT(s_context_device_extensions);

#if defined(BUILD_DEBUG)
  device_create_info.ppEnabledLayerNames = s_context_validation_layers;
  device_create_info.enabledLayerCount = CORE_ARRAY_COUNT(s_context_validation_layers);
#endif

  VK_CHECK(vkCreateDevice(g_context_physical_device, &device_create_info, 0, &g_context_device));

  vkGetDeviceQueue(g_context_device, g_context_graphics_queue_index, 0, &g_context_graphics_queue);
  vkGetDeviceQueue(g_context_device, g_context_present_queue_index, 0, &g_context_present_queue);
}
static void context_create_command_pool(void) {
  VkCommandPoolCreateInfo command_pool_create_info = {0};
  command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  command_pool_create_info.queueFamilyIndex = g_context_graphics_queue_index;

  VK_CHECK(vkCreateCommandPool(g_context_device, &command_pool_create_info, 0, &g_context_command_pool));
}

static void context_destroy_instance(void) {
#if defined(BUILD_DEBUG)
  s_context_destroy_debug_utils_messenger_ext(g_context_instance, s_context_debug_messenger, 0);
#endif

  vkDestroyInstance(g_context_instance, 0);
}
static void context_destroy_surface(void) { vkDestroySurfaceKHR(g_context_instance, g_context_surface, 0); }
static void context_destroy_device(void) { vkDestroyDevice(g_context_device, 0); }
static void context_destroy_command_pool(void) { vkDestroyCommandPool(g_context_device, g_context_command_pool, 0); }

static void context_check_surface_capabilities(void) {
  int32_t surface_format_count = 0;
  VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(g_context_physical_device, g_context_surface, &surface_format_count, 0));

  VkSurfaceFormatKHR *surface_formats = (VkSurfaceFormatKHR *)core_heap_alloc(sizeof(VkSurfaceFormatKHR) * surface_format_count);
  VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(g_context_physical_device, g_context_surface, &surface_format_count, surface_formats));

  int32_t present_mode_count = 0;
  VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(g_context_physical_device, g_context_surface, &present_mode_count, 0));

  VkPresentModeKHR *present_modes = (VkPresentModeKHR *)core_heap_alloc(sizeof(VkPresentModeKHR) * present_mode_count);
  VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(g_context_physical_device, g_context_surface, &present_mode_count, present_modes));

  uint64_t surface_format_index = 0;
  while (surface_format_index < surface_format_count) {
    VkSurfaceFormatKHR surface_format = surface_formats[surface_format_index];

    if ((surface_format.format == VK_FORMAT_B8G8R8A8_UNORM) && (surface_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)) {
      g_context_prefered_surface_format = surface_format;

      break;
    }

    surface_format_index++;
  }

  uint64_t present_mode_index = 0;
  while (present_mode_index < present_mode_count) {
    VkPresentModeKHR present_mode = present_modes[present_mode_index];

    if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
      g_context_prefered_present_mode = present_mode;

      break;
    }

    present_mode_index++;
  }

  core_heap_free(surface_formats);
  core_heap_free(present_modes);
}
static void context_check_physical_device_extensions(void) {
  int32_t available_device_extension_count = 0;
  VK_CHECK(vkEnumerateDeviceExtensionProperties(g_context_physical_device, 0, &available_device_extension_count, 0));

  VkExtensionProperties *available_device_extensions = (VkExtensionProperties *)core_heap_alloc(sizeof(VkExtensionProperties) * available_device_extension_count);
  VK_CHECK(vkEnumerateDeviceExtensionProperties(g_context_physical_device, 0, &available_device_extension_count, available_device_extensions));

  printf("Device Extensions\n");

  uint64_t device_extension_index = 0;
  uint64_t device_extension_count = CORE_ARRAY_COUNT(s_context_device_extensions);
  while (device_extension_index < device_extension_count) {
    uint8_t device_extensions_available = 0;

    uint64_t available_device_extension_index = 0;
    while (available_device_extension_index < available_device_extension_count) {
      VkExtensionProperties properties = available_device_extensions[available_device_extension_index];

      if (strcmp(s_context_device_extensions[device_extension_index], properties.extensionName) == 0) {
        printf("\tFound %s\n", s_context_device_extensions[device_extension_index]);

        device_extensions_available = 1;

        break;
      }

      available_device_extension_index++;
    }

    if (device_extensions_available == 0) {
      printf("\tMissing %s\n", s_context_device_extensions[device_extension_index]);

      break;
    }

    device_extension_index++;
  }

  printf("\n");

  core_heap_free(available_device_extensions);
}

static void context_resize_surface(void) {
  VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(g_context_physical_device, g_context_surface, &g_context_surface_capabilities));

  g_context_surface_width = g_context_surface_capabilities.currentExtent.width;
  g_context_surface_height = g_context_surface_capabilities.currentExtent.height;
}

static void context_find_physical_device(void) {
  int32_t physical_device_count = 0;
  VK_CHECK(vkEnumeratePhysicalDevices(g_context_instance, &physical_device_count, 0));

  VkPhysicalDevice *physical_devices = (VkPhysicalDevice *)core_heap_alloc(sizeof(VkPhysicalDevice) * physical_device_count);
  VK_CHECK(vkEnumeratePhysicalDevices(g_context_instance, &physical_device_count, physical_devices));

  uint64_t physical_device_index = 0;
  while (physical_device_index < physical_device_count) {
    VkPhysicalDevice physical_device = physical_devices[physical_device_index];

    vkGetPhysicalDeviceProperties(physical_device, &g_context_physical_device_properties);
    vkGetPhysicalDeviceFeatures(physical_device, &g_context_physical_device_features);
    vkGetPhysicalDeviceMemoryProperties(physical_device, &g_context_physical_device_memory_properties);

    if (g_context_physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      if (g_context_physical_device_features.geometryShader && g_context_physical_device_features.samplerAnisotropy && g_context_physical_device_features.shaderFloat64) {
        g_context_physical_device = physical_device;

        break;
      }
    }

    physical_device_index++;
  }

  core_heap_free(physical_devices);
}
static void context_find_physical_device_queue_families(void) {
  int32_t queue_family_property_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(g_context_physical_device, &queue_family_property_count, 0);

  VkQueueFamilyProperties *queue_family_properties = (VkQueueFamilyProperties *)core_heap_alloc(sizeof(VkQueueFamilyProperties) * queue_family_property_count);
  vkGetPhysicalDeviceQueueFamilyProperties(g_context_physical_device, &queue_family_property_count, queue_family_properties);

  uint64_t physical_device_queue_family_property_index = 0;
  while (physical_device_queue_family_property_index < queue_family_property_count) {
    VkQueueFamilyProperties properties = queue_family_properties[physical_device_queue_family_property_index];

    uint32_t graphics_support = 0;
    uint32_t compute_support = 0;
    uint32_t present_support = 0;

    graphics_support = properties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
    compute_support = properties.queueFlags & VK_QUEUE_COMPUTE_BIT;

    VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(g_context_physical_device, (uint32_t)physical_device_queue_family_property_index, g_context_surface, &present_support));

    if (graphics_support && compute_support && (g_context_graphics_queue_index == -1)) {
      g_context_graphics_queue_index = (uint32_t)physical_device_queue_family_property_index;
    } else if (present_support && (g_context_present_queue_index == -1)) {
      g_context_present_queue_index = (uint32_t)physical_device_queue_family_property_index;
    }

    if ((g_context_graphics_queue_index != -1) && (g_context_present_queue_index != -1)) {
      break;
    }

    physical_device_queue_family_property_index++;
  }

  core_heap_free(queue_family_properties);

  printf("Queue Indices\n");
  printf("\tGraphics Queue Index %d\n", g_context_graphics_queue_index);
  printf("\tPresent Queue Index %d\n", g_context_present_queue_index);
  printf("\n");
}
*/
#if !defined(NP_EDITOR_CONTEXT_HPP)
#  define NP_EDITOR_CONTEXT_HPP

class NpContext {
public:
  NpContext();
  virtual ~NpContext();

public:
  bool Create(int32_t Width, int32_t Height);
  bool IsRunning();
  void BeginFrame();
  void EndFrame();
  void Destroy();

private:
  void CreateInstance();
  void CreateSurface();
  void CreateDevice();
  void CreateCommandPool();

  void DestroyInstance();
  void DestroySurface();
  void DestroyDevice();
  void DestroyCommandPool();

  void CheckSurfaceCapabilities();
  void CheckPhysicalDeviceExtensions();

  void ResizeSurface();

  void FindPhysicalDevice();
  void FindPhysicalDeviceQueueFamilies();

private:
#  if defined(BUILD_DEBUG)
  std::vector<char const *> const m_ValidationLayers = {
      "VK_LAYER_KHRONOS_validation",
  };
#  endif

  std::vector<char const *> const m_LayerExtensions = {
#  if defined(BUILD_DEBUG)
      "VK_EXT_debug_utils",
#  endif
  };

  std::vector<char const *> const m_DeviceExtensions = {
      "VK_KHR_swapchain",
      "VK_EXT_descriptor_indexing",
  };

  GLFWwindow *m_Window = nullptr;

  float m_Time = 0.0F;
  float m_DeltaTime = 0.0F;

  VkInstance m_Instance = nullptr;

  VkSurfaceKHR m_Surface = 0;
  VkSurfaceCapabilitiesKHR m_SurfaceCapabilities = {};
  VkSurfaceFormatKHR m_PreferedSurfaceFormat = {};

  VkPresentModeKHR m_PreferedPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;

  int32_t m_SurfaceWidth = 0;
  int32_t m_SurfaceHeight = 0;

  VkPhysicalDevice m_PhysicalDevice = nullptr;
  VkPhysicalDeviceProperties m_PhysicalDeviceProperties = {};
  VkPhysicalDeviceFeatures m_PhysicalDeviceFeatures = {};
  VkPhysicalDeviceMemoryProperties m_PhysicalDeviceMemoryProperties = {};

  VkDevice m_Device = nullptr;

  int32_t m_GraphicsQueueIndex = -1;
  int32_t m_PresentQueueIndex = -1;

  VkQueue m_GraphicsQueue = nullptr;
  VkQueue m_PresentQueue = nullptr;

  VkCommandPool m_CommandPool = 0;

#  if defined(BUILD_DEBUG)
  PFN_vkCreateDebugUtilsMessengerEXT m_CreateDebugUtilsMessengerExt = nullptr;
  PFN_vkDestroyDebugUtilsMessengerEXT m_DestroyDebugUtilsMessengerExt = nullptr;

  VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
#  endif

  bool m_SwapchainIsDirty = false;
  bool m_RendererIsDirty = false;
};

// TODO
// static int32_t context_find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags memory_property_flags);
// static VkCommandBuffer context_begin_command_buffer(void);
// static void context_end_command_buffer(VkCommandBuffer command_buffer);

#endif

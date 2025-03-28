#pragma once

class NpEditor {
public:
  NpEditor();
  virtual ~NpEditor();

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
  GLFWwindow *m_Window = nullptr;

  float m_Time = 0.0F;
  float m_DeltaTime = 0.0F;

  VkInstance m_Instance = nullptr;

  VkSurfaceKHR m_Surface = nullptr;
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

  VkCommandPool m_CommandPool = nullptr;

#if defined(BUILD_DEBUG)
  PFN_vkCreateDebugUtilsMessengerEXT m_CreateDebugUtilsMessengerExt = nullptr;
  PFN_vkDestroyDebugUtilsMessengerEXT m_DestroyDebugUtilsMessengerExt = nullptr;

  VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
#endif

  bool m_SwapchainIsDirty = false;
  bool m_RendererIsDirty = false;
};

// TODO
// static int32_t context_find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags memory_property_flags);
// static VkCommandBuffer context_begin_command_buffer(void);
// static void context_end_command_buffer(VkCommandBuffer command_buffer);
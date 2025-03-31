#if !defined(NP_EDITOR_SWAPCHAIN_HPP)
#  define NP_EDITOR_SWAPCHAIN_HPP

class NpSwapchain {
public:
  NpSwapchain();
  virtual ~NpSwapchain();

public:
  inline auto const &GetSwapchain() const { return m_Swapchain; }
  inline auto const &GetImageCount() const { return m_ImageCount; }
  inline auto const &GetRenderPass() const { return m_RenderPass; }
  inline auto const &GetFrameBuffer() const { return m_FrameBuffer; }

public:
  void Create(uint32_t ImageCount);
  void Destroy();

private:
  void CreateRenderPass();
  void CreateColorImages();
  void CreateDepthImages();
  void CreateFrameBuffer();

  void DestroyRenderPass();
  void DestroyColorImages();
  void DestroyDepthImages();
  void DestroyFrameBuffer();

private:
  VkSwapchainKHR m_Swapchain = nullptr;

  uint32_t m_ImageCount = 0;

  VkRenderPass m_RenderPass = nullptr;

  VkFramebuffer *m_FrameBuffer = nullptr;

  VkFormat m_DepthFormat = VK_FORMAT_D32_SFLOAT;

  VkImage *m_ColorImage = nullptr;
  VkImage *m_DepthImage = nullptr;

  VkDeviceMemory *m_DepthImageDeviceMemory = nullptr;

  VkImageView *m_ColorImageView = nullptr;
  VkImageView *m_DepthImageView = nullptr;
};

extern NpSwapchain g_Swapchain;

#endif

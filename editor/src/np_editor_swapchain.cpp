#include <np_editor_pch.hpp>
#include <np_editor_context.hpp>
#include <np_editor_macros.hpp>
#include <np_editor_swapchain.hpp>

NpSwapchain g_Swapchain = {};

NpSwapchain::NpSwapchain() {}
NpSwapchain::~NpSwapchain() {}

void NpSwapchain::Create(uint32_t ImageCount) {
  m_ImageCount = ImageCount;
  m_ImageCount = std::max(m_ImageCount, g_Context.GetSurfaceCapabilities().minImageCount);
  m_ImageCount = std::min(m_ImageCount, g_Context.GetSurfaceCapabilities().maxImageCount);

  VkSwapchainCreateInfoKHR SwapchainCreateInfo = {};
  SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  SwapchainCreateInfo.surface = g_Context.GetSurface();
  SwapchainCreateInfo.minImageCount = m_ImageCount;
  SwapchainCreateInfo.imageFormat = g_Context.GetPreferedSurfaceFormat().format;
  SwapchainCreateInfo.imageColorSpace = g_Context.GetPreferedSurfaceFormat().colorSpace;
  SwapchainCreateInfo.imageExtent.width = g_Context.GetSurfaceWidth();
  SwapchainCreateInfo.imageExtent.height = g_Context.GetSurfaceHeight();
  SwapchainCreateInfo.imageArrayLayers = 1;
  SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  SwapchainCreateInfo.preTransform = g_Context.GetSurfaceCapabilities().currentTransform;
  SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  SwapchainCreateInfo.presentMode = g_Context.GetPreferedPresentMode();
  SwapchainCreateInfo.clipped = 1;
  SwapchainCreateInfo.oldSwapchain = nullptr;

  std::array<uint32_t, 2> QueueFamilies = {(uint32_t)g_Context.GetGraphicsQueueIndex(), (uint32_t)g_Context.GetPresentQueueIndex()};

  if (g_Context.GetGraphicsQueueIndex() == g_Context.GetPresentQueueIndex()) {
    SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    SwapchainCreateInfo.queueFamilyIndexCount = 0;
    SwapchainCreateInfo.pQueueFamilyIndices = nullptr;
  } else {
    SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    SwapchainCreateInfo.pQueueFamilyIndices = QueueFamilies.data();
    SwapchainCreateInfo.queueFamilyIndexCount = QueueFamilies.size();
  }

  VK_CHECK(vkCreateSwapchainKHR(g_Context.GetDevice(), &SwapchainCreateInfo, nullptr, &m_Swapchain));

  CreateRenderPass();
  CreateColorImages();
  CreateDepthImages();
  CreateFrameBuffer();
}
void NpSwapchain::Destroy() {
  DestroyFrameBuffer();
  DestroyDepthImages();
  DestroyColorImages();
  DestroyRenderPass();

  vkDestroySwapchainKHR(g_Context.GetDevice(), m_Swapchain, nullptr);
}

void NpSwapchain::CreateRenderPass() {
  VkAttachmentDescription ColorAttachmentDescription = {};
  ColorAttachmentDescription.format = g_Context.GetPreferedSurfaceFormat().format;
  ColorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
  ColorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  ColorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  ColorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  ColorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  ColorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  ColorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentDescription DepthAttachmentDescription = {};
  DepthAttachmentDescription.format = m_DepthFormat;
  DepthAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
  DepthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  DepthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  DepthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  DepthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  DepthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  DepthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference ColorAttachmentReference = {};
  ColorAttachmentReference.attachment = 0;
  ColorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference DepthAttachmentReference = {};
  DepthAttachmentReference.attachment = 1;
  DepthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription SubpassDescription = {};
  SubpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  SubpassDescription.colorAttachmentCount = 1;
  SubpassDescription.pColorAttachments = &ColorAttachmentReference;
  SubpassDescription.pDepthStencilAttachment = &DepthAttachmentReference;

  VkSubpassDependency SubpassDependency = {};
  SubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  SubpassDependency.dstSubpass = 0;
  SubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  SubpassDependency.srcAccessMask = VK_ACCESS_NONE;
  SubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  SubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 2> AttachmentDescriptions = {ColorAttachmentDescription, DepthAttachmentDescription};

  VkRenderPassCreateInfo RenderPassCreateInfo = {};
  RenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  RenderPassCreateInfo.pAttachments = AttachmentDescriptions.data();
  RenderPassCreateInfo.attachmentCount = AttachmentDescriptions.size();
  RenderPassCreateInfo.pSubpasses = &SubpassDescription;
  RenderPassCreateInfo.subpassCount = 1;
  RenderPassCreateInfo.pDependencies = &SubpassDependency;
  RenderPassCreateInfo.dependencyCount = 1;

  VK_CHECK(vkCreateRenderPass(g_Context.GetDevice(), &RenderPassCreateInfo, nullptr, &m_RenderPass));
}
void NpSwapchain::CreateColorImages() {
  m_ColorImage = new VkImage[m_ImageCount];
  m_ColorImageView = new VkImageView[m_ImageCount];

  VK_CHECK(vkGetSwapchainImagesKHR(g_Context.GetDevice(), m_Swapchain, &m_ImageCount, m_ColorImage));

  for (uint32_t ImageIndex = 0; ImageIndex < m_ImageCount; ImageIndex++) {
    VkImageViewCreateInfo ImageViewCreateInfo = {};
    ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ImageViewCreateInfo.image = m_ColorImage[ImageIndex];
    ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ImageViewCreateInfo.format = g_Context.GetPreferedSurfaceFormat().format;
    ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    ImageViewCreateInfo.subresourceRange.levelCount = 1;
    ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    ImageViewCreateInfo.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(g_Context.GetDevice(), &ImageViewCreateInfo, nullptr, &m_ColorImageView[ImageIndex]));
  }
}
void NpSwapchain::CreateDepthImages() {
  m_DepthImage = new VkImage[m_ImageCount];
  m_DepthImageDeviceMemory = new VkDeviceMemory[m_ImageCount];
  m_DepthImageView = new VkImageView[m_ImageCount];

  for (uint32_t ImageIndex = 0; ImageIndex < m_ImageCount; ImageIndex++) {
    VkImageCreateInfo ImageCreateInfo = {};
    ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    ImageCreateInfo.extent.width = g_Context.GetSurfaceWidth();
    ImageCreateInfo.extent.height = g_Context.GetSurfaceHeight();
    ImageCreateInfo.extent.depth = 1;
    ImageCreateInfo.mipLevels = 1;
    ImageCreateInfo.arrayLayers = 1;
    ImageCreateInfo.format = m_DepthFormat;
    ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ImageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateImage(g_Context.GetDevice(), &ImageCreateInfo, nullptr, &m_DepthImage[ImageIndex]));

    VkMemoryRequirements MemoryRequirements = {};

    vkGetImageMemoryRequirements(g_Context.GetDevice(), m_DepthImage[ImageIndex], &MemoryRequirements);

    uint32_t MemoryTypeIndex = g_Context.FindMemoryType(MemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkMemoryAllocateInfo MemoryAllocateInfo = {};
    MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
    MemoryAllocateInfo.memoryTypeIndex = MemoryTypeIndex;

    VK_CHECK(vkAllocateMemory(g_Context.GetDevice(), &MemoryAllocateInfo, nullptr, &m_DepthImageDeviceMemory[ImageIndex]));
    VK_CHECK(vkBindImageMemory(g_Context.GetDevice(), m_DepthImage[ImageIndex], m_DepthImageDeviceMemory[ImageIndex], 0));

    VkImageViewCreateInfo ImageViewCreateInfo = {};
    ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ImageViewCreateInfo.image = m_DepthImage[ImageIndex];
    ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ImageViewCreateInfo.format = m_DepthFormat;
    ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    ImageViewCreateInfo.subresourceRange.levelCount = 1;
    ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    ImageViewCreateInfo.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(g_Context.GetDevice(), &ImageViewCreateInfo, nullptr, &m_DepthImageView[ImageIndex]));
  }
}
void NpSwapchain::CreateFrameBuffer() {
  m_FrameBuffer = new VkFramebuffer[m_ImageCount];

  for (uint32_t ImageIndex = 0; ImageIndex < m_ImageCount; ImageIndex++) {
    std::array<VkImageView, 2> ImageAttachments = {m_ColorImageView[ImageIndex], m_DepthImageView[ImageIndex]};

    VkFramebufferCreateInfo FrameBufferCreateInfo = {};
    FrameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    FrameBufferCreateInfo.renderPass = m_RenderPass;
    FrameBufferCreateInfo.pAttachments = ImageAttachments.data();
    FrameBufferCreateInfo.attachmentCount = ImageAttachments.size();
    FrameBufferCreateInfo.width = g_Context.GetSurfaceWidth();
    FrameBufferCreateInfo.height = g_Context.GetSurfaceHeight();
    FrameBufferCreateInfo.layers = 1;

    VK_CHECK(vkCreateFramebuffer(g_Context.GetDevice(), &FrameBufferCreateInfo, nullptr, &m_FrameBuffer[ImageIndex]));
  }
}

void NpSwapchain::DestroyRenderPass() {
  vkDestroyRenderPass(g_Context.GetDevice(), m_RenderPass, nullptr);
}
void NpSwapchain::DestroyColorImages() {
  for (uint32_t ImageIndex = 0; ImageIndex < m_ImageCount; ImageIndex++) {
    vkDestroyImageView(g_Context.GetDevice(), m_ColorImageView[ImageIndex], 0);
  }

  delete[] m_ColorImage;
  delete[] m_ColorImageView;
}
void NpSwapchain::DestroyDepthImages() {
  for (uint32_t ImageIndex = 0; ImageIndex < m_ImageCount; ImageIndex++) {
    vkDestroyImageView(g_Context.GetDevice(), m_DepthImageView[ImageIndex], nullptr);
    vkFreeMemory(g_Context.GetDevice(), m_DepthImageDeviceMemory[ImageIndex], nullptr);
    vkDestroyImage(g_Context.GetDevice(), m_DepthImage[ImageIndex], nullptr);
  }

  delete[] m_DepthImage;
  delete[] m_DepthImageDeviceMemory;
  delete[] m_DepthImageView;
}
void NpSwapchain::DestroyFrameBuffer() {
  for (uint32_t ImageIndex = 0; ImageIndex < m_ImageCount; ImageIndex++) {
    vkDestroyFramebuffer(g_Context.GetDevice(), m_FrameBuffer[ImageIndex], 0);
  }

  delete[] m_FrameBuffer;
}
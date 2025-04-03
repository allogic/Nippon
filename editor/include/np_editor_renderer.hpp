#if !defined(NP_EDITOR_RENDERER_HPP)
#  define NP_EDITOR_RENDERER_HPP

#  include <np_editor_camera_component.hpp>
#  include <np_editor_transform_component.hpp>

struct NpTimeInfo {
  float Time;
  float DeltaTime;
};

struct NpScreenInfo {
  float Width;
  float Height;
};

struct NpCameraInfo {
  glm::fvec3 WorldPosition;
  float Reserved;
  glm::fmat4 View;
  glm::fmat4 Projection;
  glm::fmat4 ViewProjection;
  glm::fmat4 ViewProjectionInv;
};

struct NpDefaultObjectVertex {
  glm::fvec3 Position;
  glm::fvec4 Color;
};

typedef uint32_t NpDefaultObjectIndex;

struct NpDebugLineVertex {
  glm::fvec3 Position;
  glm::fvec4 Color;
};

typedef uint32_t NpDebugLineIndex;

class NpRenderer {
public:
  NpRenderer();
  virtual ~NpRenderer();

public:
  void Create(uint32_t FramesInFlight);
  void Draw(NpTransformComponent const &Transform, NpCameraComponent const &Camera);
  void Destroy();

  void DrawDebugLine(glm::fvec3 const &From, glm::fvec3 const &To, glm::fvec4 const &Color);
  void DrawDebugBox(glm::fvec3 const &Position, glm::fvec3 const &Size, glm::fvec4 const &Color);

private:
  void CreateCommandBuffer();
  void CreateSyncObject();
  void CreateDescriptorPool();
  void CreateDescriptorSetLayout();
  void CreateDescriptorSet();
  void CreatePipelineLayout();
  void CreateImGui();

  void CreateDefaultObjectPipeline();
  void CreateDebugLinePipeline();

  void CreateTimeBuffer();
  void CreateScreenBuffer();
  void CreateCameraBuffer();
  void CreateDefaultObjectVertexBuffer();
  void CreateDefaultObjectIndexBuffer();
  void CreateDebugLineVertexBuffer();
  void CreateDebugLineIndexBuffer();

  void UpdateDefaultObjectDescriptorSet();
  void UpdateDebugLineDescriptorSet();

  void UpdateUniformBuffer(NpTransformComponent const &Transform, NpCameraComponent const &Camera);

  void RecordGraphicsCommand();

  void DestroyCommandBuffer();
  void DestroySyncObject();
  void DestroyDescriptorPool();
  void DestroyDescriptorSetLayout();
  void DestroyDescriptorSet();
  void DestroyPipelineLayout();
  void DestroyPipeline();
  void DestroyImGui();

  void DestroyTimeBuffer();
  void DestroyScreenBuffer();
  void DestroyCameraBuffer();
  void DestroyDefaultObjectVertexBuffer();
  void DestroyDefaultObjectIndexBuffer();
  void DestroyDebugLineVertexBuffer();
  void DestroyDebugLineIndexBuffer();

private:
  std::vector<VkVertexInputBindingDescription> const m_DefaultObjectVertexInputBindingDescription = {
      {0, sizeof(NpDefaultObjectVertex), VK_VERTEX_INPUT_RATE_VERTEX},
  };
  std::vector<VkVertexInputBindingDescription> const m_DebugLineVertexInputBindingDescription = {
      {0, sizeof(NpDebugLineVertex), VK_VERTEX_INPUT_RATE_VERTEX},
  };

  std::vector<VkVertexInputAttributeDescription> const m_DefaultObjectVertexInputAttributeDescription = {
      {0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0},
      {1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(NpDefaultObjectVertex, Color)},
  };
  std::vector<VkVertexInputAttributeDescription> const m_DebugLineVertexInputAttributeDescription = {
      {0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0},
      {1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(NpDebugLineVertex, Color)},
  };

  VkCommandBuffer *m_GraphicsCommandBuffer = nullptr;

  VkSemaphore *m_GraphicsCompleteSemaphore = nullptr;
  VkSemaphore *m_PresentCompleteSemaphore = nullptr;

  VkFence *m_FrameFence = nullptr;

  uint32_t m_FramesInFlight = 0;
  uint32_t m_FrameIndex = 0;
  uint32_t m_ImageIndex = 0;

  VkDescriptorPool m_DefaultObjectDescriptorPool = nullptr;
  VkDescriptorPool m_DebugLineDescriptorPool = nullptr;
  VkDescriptorPool m_ImGuiDescriptorPool = nullptr;

  VkDescriptorSetLayout m_DefaultObjectDescriptorSetLayout = nullptr;
  VkDescriptorSetLayout m_DebugLineDescriptorSetLayout = nullptr;

  VkPipelineLayout m_DefaultObjectPipelineLayout = nullptr;
  VkPipelineLayout m_DebugLinePipelineLayout = nullptr;

  VkPipeline m_DefaultObjectPipeline = nullptr;
  VkPipeline m_DebugLinePipeline = nullptr;

  VkDescriptorSet *m_DefaultObjectDescriptorSet = nullptr;
  VkDescriptorSet *m_DebugLineDescriptorSet = nullptr;

  VkBuffer *m_TimeBuffer = nullptr;
  VkBuffer *m_ScreenBuffer = nullptr;
  VkBuffer *m_CameraBuffer = nullptr;
  VkBuffer *m_DefaultObjectVertexBuffer = nullptr;
  VkBuffer *m_DefaultObjectIndexBuffer = nullptr;
  VkBuffer *m_DebugLineVertexBuffer = nullptr;
  VkBuffer *m_DebugLineIndexBuffer = nullptr;

  VkDeviceMemory *m_TimeBufferDeviceMemory = nullptr;
  VkDeviceMemory *m_ScreenBufferDeviceMemory = nullptr;
  VkDeviceMemory *m_CameraBufferDeviceMemory = nullptr;
  VkDeviceMemory *m_DefaultObjectVertexBufferDeviceMemory = nullptr;
  VkDeviceMemory *m_DefaultObjectIndexBufferDeviceMemory = nullptr;
  VkDeviceMemory *m_DebugLineVertexBufferDeviceMemory = nullptr;
  VkDeviceMemory *m_DebugLineIndexBufferDeviceMemory = nullptr;

  NpTimeInfo **m_Time = nullptr;
  NpScreenInfo **m_Screen = nullptr;
  NpCameraInfo **m_Camera = nullptr;
  NpDefaultObjectVertex **m_DefaultObjectVertex = nullptr;
  NpDefaultObjectIndex **m_DefaultObjectIndex = nullptr;
  NpDebugLineVertex **m_DebugLineVertex = nullptr;
  NpDebugLineIndex **m_DebugLineIndex = nullptr;

  uint32_t *m_DebugLineVertexOffset = nullptr;
  uint32_t *m_DebugLineIndexOffset = nullptr;
};

extern NpRenderer g_Renderer;

#endif

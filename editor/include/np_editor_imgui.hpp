#if !defined(NP_EDITOR_IMGUI_HPP)
#  define NP_EDITOR_IMGUI_HPP

class NpImGui {
public:
  NpImGui();
  virtual ~NpImGui();

  void Create();
  void Draw(VkCommandBuffer CommandBuffer);
  void Destroy();

private:
  VkDescriptorPool m_DescriptorPool = nullptr;

  ImGuiContext *m_Context = nullptr;
};

#endif

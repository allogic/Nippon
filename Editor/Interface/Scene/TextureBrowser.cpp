#include <Editor/Scene.h>
#include <Editor/Texture.h>

#include <Editor/Interface/Scene/TextureBrowser.h>

#include <Vendor/ImGui/imgui.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern ark::Scene* gScene;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  void TextureBrowser::Reset()
  {

  }

  void TextureBrowser::Draw()
  {
    ImGui::Begin("Texture Browser");

    if (gScene)
    {
      ImGui::Columns(4);

      const auto& textures = gScene->GetTextures();

      U32 size = (U32)textures.size();

      ImVec2 view = ImGui::GetContentRegionAvail();

      for (U32 i = 0; i < size; i++)
      {
        if ((i > 0) && ((i % 4) == 0))
        {
          ImGui::Separator();
        }

        R32 width = std::min(view.x, (R32)textures[i]->GetWidth());
        R32 height = std::min(view.y, (R32)textures[i]->GetHeight());

        ImGui::Image((void*)(U64)textures[i]->GetId(), ImVec2{ width, (R32)textures[i]->GetHeight() }, ImVec2{ 0.0F, 1.0F }, ImVec2{ 1.0F, 0.0F });
        ImGui::NextColumn();
      }

      ImGui::Columns(1);
    }

    ImGui::End();
  }
}
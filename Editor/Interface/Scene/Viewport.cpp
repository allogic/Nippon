#include <Common/Debug.h>

#include <Editor/Scene.h>
#include <Editor/Texture.h>

#include <Editor/Interface/Scene/Viewport.h>

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
  void Viewport::Reset()
  {

  }

  void Viewport::Draw()
  {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0F, 0.0F });

    ImGui::Begin("Viewport");

    if (gScene)
    {
      if (HasResized())
      {
        gScene->Resize(mWidth, mHeight);
      }

      ImGui::Image((void*)(U64)gScene->GetFrameBuffer().GetColorTexture()->GetId(), ImVec2{ (R32)mWidth, (R32)mHeight }, ImVec2{ 0.0F, 1.0F }, ImVec2{ 1.0F, 0.0F });
    }

    ImGui::End();

    ImGui::PopStyleVar();
  }

  bool Viewport::HasResized()
  {
    ImVec2 view = ImGui::GetContentRegionAvail();

    if ((U32)view.x != mWidth || (U32)view.y != mHeight)
    {
      if ((U32)view.x == 0 || (U32)view.y == 0)
      {
        return false;
      }

      mWidth = (U32)view.x;
      mHeight = (U32)view.y;

      return true;
    }

    return false;
  }
}
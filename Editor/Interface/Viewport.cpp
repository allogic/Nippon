#include <Common/Debug.h>

#include <Editor/Scene.h>
#include <Editor/Texture.h>

#include <Editor/Interface/Viewport.h>

#include <Vendor/ImGui/imgui.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  Viewport::Viewport(Scene* Scene)
    : mScene{ Scene }
  {

  }

  void Viewport::Reset()
  {
    mIsFocused = false;
  }

  void Viewport::Draw()
  {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0F, 0.0F });

    ImGui::Begin(mScene->GetName().c_str());

    mIsFocused = ImGui::IsWindowFocused();

    if (HasResized())
    {
      mScene->Resize(mWidth, mHeight);
    }
    
    if (mIsFocused)
    {
      mScene->Update();
      mScene->Render();
    }

    ImGui::Image((void*)(U64)mScene->GetFrameBuffer().GetColorTexture()->GetId(), ImVec2{ (R32)mWidth, (R32)mHeight }, ImVec2{ 0.0F, 1.0F }, ImVec2{ 1.0F, 0.0F });

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
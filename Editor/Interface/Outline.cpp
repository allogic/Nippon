#include <Editor/Actor.h>
#include <Editor/Scene.h>

#include <Editor/Components/Transform.h>

#include <Editor/Interface/Outline.h>

#include <Vendor/ImGui/imgui.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  void Outline::Reset()
  {
    mSelectedActor = nullptr;
  }

  void Outline::Draw()
  {
    ImGui::Begin("Outline");

    //if (gScene)
    //{
    //  for (auto& actor : gScene->GetActors())
    //  {
    //    if (!actor->HasParent())
    //    {
    //      DrawActorRecursive(actor);
    //    }
    //  }
    //}

    ImGui::End();
  }

  void Outline::DrawActorRecursive(Actor* Actor)
  {
    ImGui::PushID(Actor);

    U32 flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap;

    if (Actor == mSelectedActor) flags |= ImGuiTreeNodeFlags_Selected;

    if (Actor->IsChild())
    {
      flags |= ImGuiTreeNodeFlags_Leaf;
    }

    U32 opened = ImGui::TreeNodeEx(Actor->GetName().c_str(), flags);

    if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1))
    {
      mSelectedActor = Actor;
    }

    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 15.0F);
    if (ImGui::Button("x", ImVec2{ 15.0F, 12.0F }))
    {

    }
    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 35.0F);
    if (ImGui::Button("-", ImVec2{ 15.0F, 12.0F }))
    {
      Actor->SetActive(!Actor->IsActive());
    }

    if (opened)
    {
      for (auto& child : *Actor)
      {
        DrawActorRecursive(child);
      }

      ImGui::TreePop();
    }

    ImGui::PopID();
  }
}
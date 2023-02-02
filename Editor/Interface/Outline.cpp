#include <Editor/Actor.h>
#include <Editor/Scene.h>

#include <Editor/Components/Transform.h>

#include <Editor/Interface/Outline.h>

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
  void Outline::Draw()
  {
    ImGui::Begin("Outline");

    if (gScene)
    {
      for (auto& actor : gScene->GetActors())
      {
        if (actor->HasNoParent())
        {
          DrawActorRecursive(actor);
        }
      }
    }

    ImGui::End();
  }

  void Outline::DrawActorRecursive(Actor* Actor)
  {
    ImGui::PushID(Actor);

    std::uint32_t flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;

    if (Actor == mSelectedActor) flags |= ImGuiTreeNodeFlags_Selected;

    if (Actor->IsChild())
    {
      flags |= ImGuiTreeNodeFlags_Leaf;
    }

    std::uint32_t opened = ImGui::TreeNodeEx(Actor->GetName().c_str(), flags);

    if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1))
    {
      mSelectedActor = Actor;
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
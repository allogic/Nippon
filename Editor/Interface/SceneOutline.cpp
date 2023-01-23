#include <Editor/Actor.h>
#include <Editor/Scene.h>

#include <Editor/Components/Transform.h>

#include <Editor/Interface/SceneOutline.h>

#include <Vendor/ImGui/imgui.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  void SceneOutline::Update()
  {

  }

  void SceneOutline::Draw()
  {
    ImGui::Begin("Scene Outline");

    for (auto& actor : Scene::GetActors())
    {
      if (actor->HasNoParent())
      {
        DrawActorRecursive(actor);
      }
    }

    ImGui::End();
  }

  void SceneOutline::DrawActorRecursive(Actor* Actor)
  {
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
      ImGui::Text("Position: [%f,%f,%f]", Actor->GetTransform()->GetWorldPosition().x, Actor->GetTransform()->GetWorldPosition().y, Actor->GetTransform()->GetWorldPosition().z);
      ImGui::Text("Rotation: [%f,%f,%f]", Actor->GetTransform()->GetWorldRotation().x, Actor->GetTransform()->GetWorldRotation().y, Actor->GetTransform()->GetWorldRotation().z);
      ImGui::Text("Scale   : [%f,%f,%f]", Actor->GetTransform()->GetWorldScale().x, Actor->GetTransform()->GetWorldScale().y, Actor->GetTransform()->GetWorldScale().z);

      for (auto& child : *Actor)
      {
        DrawActorRecursive(child);
      }

      ImGui::TreePop();
    }
  }
}
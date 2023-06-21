#include <Editor/Scene.h>
#include <Editor/Texture.h>
#include <Editor/Actor.h>

#include <Editor/Components/Camera.h>
#include <Editor/Components/Transform.h>
#include <Editor/Components/Renderable.h>

#include <Editor/Interface/Scene/Inspector.h>
#include <Editor/Interface/Scene/Outline.h>

#include <Vendor/ImGui/imgui.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern ark::Scene* gScene;

extern ark::Outline* gOutline;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  void Inspector::Reset()
  {

  }

  void Inspector::Draw()
  {
    ImGui::Begin("Inspector");

    if (gScene)
    {
      Actor* actor = gOutline->GetSelectedActor();

      if (actor)
      {
        if (Transform* transform = actor->GetComponent<Transform>())
        {
          ImGui::PushID(transform);
          if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen))
          {
            ImGui::PushItemWidth(ImGui::GetWindowWidth() - ImGui::GetTreeNodeToLabelSpacing() - 80);

            R32V3 localPosition = transform->GetLocalPosition();
            if (ImGui::DragFloat3("Position", &localPosition[0], 0.1F, -360.0F, 360.0F, "%.3F", 1.0F))
            {
              transform->SetLocalPosition(localPosition);
            }

            R32V3 eulerAngles = transform->GetLocalEulerAngles();
            if (ImGui::DragFloat3("Rotation", &eulerAngles[0], 0.1F, -360.0F, 360.0F, "%.3F", 1.0F))
            {
              transform->SetLocalRotation(eulerAngles);
            }

            R32V3 localScale = transform->GetLocalScale();
            if (ImGui::DragFloat3("Scale", &localScale[0], 0.1F, -360.0F, 360.0F, "%.3F", 1.0F))
            {
              transform->SetLocalScale(localScale);
            }

            ImGui::PopItemWidth();
            ImGui::TreePop();
          }
          ImGui::PopID();
        }

        if (Camera* camera = actor->GetComponent<Camera>())
        {
          ImGui::PushID(camera);
          if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen))
          {
            ImGui::PushItemWidth(ImGui::GetWindowWidth() - ImGui::GetTreeNodeToLabelSpacing() - 80);

            R32 fov = camera->GetFov();
            if (ImGui::DragFloat("Fov", &fov, 0.1F, 0.1F, 90.0F, "%.3F"))
            {
              camera->SetFov(fov);
            }

            R32 near = camera->GetNear();
            if (ImGui::DragFloat("Near", &near, 0.1F, 0.1F, 100.0F, "%.3F"))
            {
              camera->SetNear(near);
            }

            R32 far = camera->GetFar();
            if (ImGui::DragFloat("Far", &far, 1.0F, 100.0F, 100000.0F, "%.3F"))
            {
              camera->SetFar(far);
            }

            ImGui::PopItemWidth();
            ImGui::TreePop();
          }
          ImGui::PopID();
        }

        if (Renderable* renderable = actor->GetComponent<Renderable>())
        {
          
        }
      }
    }

    ImGui::End();
  }
}
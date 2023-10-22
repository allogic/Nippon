#include <Editor/Texture2D.h>
#include <Editor/Actor.h>
#include <Editor/Scene.h>
#include <Editor/SceneManager.h>
#include <Editor/InterfaceManager.h>

#include <Editor/Components/Camera.h>
#include <Editor/Components/CameraController.h>
#include <Editor/Components/Transform.h>
#include <Editor/Components/Renderable.h>

#include <Editor/Interface/Inspector.h>
#include <Editor/Interface/Outline.h>

#include <Editor/ImGui/imgui.h>

namespace ark
{
	void Inspector::Reset()
	{

	}

	void Inspector::Render()
	{
		ImGui::Begin("Inspector");

		if (Scene* scene = SceneManager::GetActiveScene())
		{		
			if (Actor* actor = InterfaceManager::GetOutline()->GetSelectedActor())
			{
				if (Transform* transform = actor->GetComponent<Transform>())
				{
					ImGui::PushID(transform);
					if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding))
					{
						ImGui::PushItemWidth(ImGui::GetWindowWidth() - ImGui::GetTreeNodeToLabelSpacing() - 100);

						R32 r32Max = std::numeric_limits<R32>::max();

						R32V3 localPosition = transform->GetLocalPosition();
						if (ImGui::DragFloat3("Position", &localPosition[0], 0.1F, -r32Max, r32Max, "%.3F", 0))
						{
							transform->SetLocalPosition(localPosition);

							actor->ComputeAABB();

							scene->Invalidate();
						}

						R32V3 eulerAngles = transform->GetLocalEulerAngles();
						if (ImGui::DragFloat3("Rotation", &eulerAngles[0], 0.1F, -r32Max, r32Max, "%.3F", 0))
						{
							transform->SetLocalRotation(eulerAngles);

							actor->ComputeAABB();

							scene->Invalidate();
						}

						R32V3 localScale = transform->GetLocalScale();
						if (ImGui::DragFloat3("Scale", &localScale[0], 0.1F, -r32Max, r32Max, "%.3F", 0))
						{
							transform->SetLocalScale(localScale);

							actor->ComputeAABB();

							scene->Invalidate();
						}

						ImGui::PopItemWidth();
						ImGui::TreePop();
					}
					ImGui::PopID();
				}
		
				if (Camera* camera = actor->GetComponent<Camera>())
				{
					ImGui::PushID(camera);
					if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding))
					{
						ImGui::PushItemWidth(ImGui::GetWindowWidth() - ImGui::GetTreeNodeToLabelSpacing() - 100);

						R32 fov = camera->GetFov();
						if (ImGui::DragFloat("Fov", &fov, 0.1F, 0.1F, 90.0F, "%.3F"))
						{
							camera->SetFov(fov);

							scene->Invalidate();
						}

						R32 near = camera->GetNear();
						if (ImGui::DragFloat("Near", &near, 0.1F, 0.1F, 100.0F, "%.3F"))
						{
							camera->SetNear(near);

							scene->Invalidate();
						}

						R32 far = camera->GetFar();
						if (ImGui::DragFloat("Far", &far, 1.0F, 100.0F, 100000.0F, "%.3F"))
						{
							camera->SetFar(far);

							scene->Invalidate();
						}

						ImGui::PopItemWidth();
						ImGui::TreePop();
					}
					ImGui::PopID();
				}

				if (CameraController* cameraController = actor->GetComponent<CameraController>())
				{
					ImGui::PushID(cameraController);
					if (ImGui::TreeNodeEx("CameraController", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding))
					{
						ImGui::PushItemWidth(ImGui::GetWindowWidth() - ImGui::GetTreeNodeToLabelSpacing() - 220);

						R32 keyboardMovementSpeedNormal = cameraController->GetKeyboardMovementSpeedNormal();
						if (ImGui::DragFloat("KeyboardMovementSpeedNormal", &keyboardMovementSpeedNormal, 0.1F, 0.0F, 100.0F, "%.3F"))
						{
							cameraController->SetKeyboardMovementSpeedNormal(keyboardMovementSpeedNormal);
						}

						R32 keyboardMovementSpeedFast = cameraController->GetKeyboardMovementSpeedFast();
						if (ImGui::DragFloat("KeyboardMovementSpeedFast", &keyboardMovementSpeedFast, 1.0F, 0.0F, 1000.0F, "%.3F"))
						{
							cameraController->SetKeyboardMovementSpeedFast(keyboardMovementSpeedFast);
						}

						R32 mouseMovementSpeedNormal = cameraController->GetMouseMovementSpeedNormal();
						if (ImGui::DragFloat("MouseMovementSpeedNormal", &mouseMovementSpeedNormal, 0.01F, 0.0F, 1.0F, "%.3F"))
						{
							cameraController->SetMouseMovementSpeedNormal(mouseMovementSpeedNormal);
						}

						R32 mouseMovementSpeedFast = cameraController->GetMouseMovementSpeedFast();
						if (ImGui::DragFloat("MouseMovementSpeedFast", &mouseMovementSpeedFast, 0.01F, 0.0F, 1.0F, "%.3F"))
						{
							cameraController->SetMouseMovementSpeedFast(mouseMovementSpeedFast);
						}

						R32 mouseRotationSpeed = cameraController->GetMouseRotationSpeed();
						if (ImGui::DragFloat("MouseRotationSpeed", &mouseRotationSpeed, 0.01F, 0.0F, 10.0F, "%.3F"))
						{
							cameraController->SetMouseRotationSpeed(mouseRotationSpeed);
						}

						R32 mouseDragDamping = cameraController->GetMouseDragDamping();
						if (ImGui::DragFloat("MouseDragDamping", &mouseDragDamping, 0.01F, 0.0F, 100.0F, "%.3F"))
						{
							cameraController->SetMouseDragDamping(mouseDragDamping);
						}

						ImGui::PopItemWidth();
						ImGui::TreePop();
					}
					ImGui::PopID();
				}

				if (Renderable* renderable = actor->GetComponent<Renderable>())
				{
					ImGui::PushID(renderable);
					if (ImGui::TreeNodeEx("Renderable", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding))
					{
						ImGui::PushItemWidth(ImGui::GetWindowWidth() - ImGui::GetTreeNodeToLabelSpacing() - 100);

						ImGui::Text("Vertices: %u", (U32)renderable->GetVertexBuffer().size());
						ImGui::Text("Indices: %u", (U32)renderable->GetElementBuffer().size());
						ImGui::Text("Texture Index: %u", renderable->GetTextureIndex());

						//if (ImGui::Combo())
						//{
						//
						//}

						ImGui::PopItemWidth();
						ImGui::TreePop();
					}
					ImGui::PopID();
				}
			}
		}

		ImGui::End();
	}
}

#include <Font/MaterialDesignIcons.h>

#include <Interface/Inspector.h>

#include <Ecs/Entity.h>

#include <Ecs/Components/Camera.h>
#include <Ecs/Components/CameraController.h>
#include <Ecs/Components/Renderable.h>
#include <Ecs/Components/Transform.h>

#include <Interface/Outline.h>

#include <ImGui/imgui.h>

#include <OpenGl/StaticMesh.h>

#include <Scene/Scene.h>
#include <Scene/SceneManager.h>

namespace Nippon
{
	void Inspector::Reset()
	{

	}

	void Inspector::Render()
	{
		ImGui::Begin(ICON_MDI_PLAYLIST_EDIT " Inspector");

		if (Scene* scene = SceneManager::GetCurrentScene())
		{
			if (Entity* entity = Outline::GetSelectedEntity())
			{
				if (Transform* transform = entity->GetComponent<Transform>())
				{
					ImGui::PushID(transform);
					if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding))
					{
						ImGui::PushItemWidth(ImGui::GetWindowWidth() - ImGui::GetTreeNodeToLabelSpacing() - 100);

						R32 r32Max = std::numeric_limits<R32>::max();
						R32 r32Min = std::numeric_limits<R32>::min();

						R32V3 localPosition = transform->GetLocalPosition();
						if (ImGui::DragFloat3("Position", &localPosition[0], 0.1F, r32Min, r32Max, "%.3F", 0))
						{
							transform->SetPosition(localPosition);

							entity->ComputeAABB();

							scene->Invalidate();
						}

						R32V3 eulerAngles = transform->GetLocalEulerAngles();
						if (ImGui::DragFloat3("Rotation", &eulerAngles[0], 0.1F, r32Min, r32Max, "%.3F", 0))
						{
							transform->SetRotation(eulerAngles);

							entity->ComputeAABB();

							scene->Invalidate();
						}

						R32V3 localScale = transform->GetLocalScale();
						if (ImGui::DragFloat3("Scale", &localScale[0], 0.1F, r32Min, r32Max, "%.3F", 0))
						{
							transform->SetScale(localScale);

							entity->ComputeAABB();

							scene->Invalidate();
						}

						ImGui::PopItemWidth();
						ImGui::TreePop();
					}
					ImGui::PopID();
				}

				if (Camera* camera = entity->GetComponent<Camera>())
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

				if (CameraController* cameraController = entity->GetComponent<CameraController>())
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

				if (Renderable* renderable = entity->GetComponent<Renderable>())
				{
					ImGui::PushID(renderable);
					if (ImGui::TreeNodeEx("Renderable", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding))
					{
						ImGui::PushItemWidth(ImGui::GetWindowWidth() - ImGui::GetTreeNodeToLabelSpacing() - 100);

						StaticMesh* staticMesh = renderable->GetStaticMesh();

						ImGui::Text("Vertices: %llu", staticMesh->GetVertexBufferCount());
						ImGui::Text("Indices: %llu", staticMesh->GetIndexBufferCount());
						ImGui::Text("Texture Index: %u", renderable->GetTextureIndex());

						// TODO

						U32 currModel = 0;
						U32 currTexture = 0;

						//if (ImGui::Combo("Model", &currModel, scene->GetModelList()))
						//{
						//
						//}

						std::string selectedTexture = std::to_string(renderable->GetTexture());

						if (ImGui::BeginCombo("Model", selectedTexture.data()))
						{
							//for (const auto& texture : scene->GetTextures())
							//{
							//
							//}

							ImGui::EndCombo();
						}

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
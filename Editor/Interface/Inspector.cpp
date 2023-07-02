#include <Editor/Texture.h>
#include <Editor/Actor.h>
#include <Editor/Scene.h>
#include <Editor/SceneManager.h>
#include <Editor/InterfaceManager.h>
#include <Editor/Math.h>

#include <Editor/Components/Camera.h>
#include <Editor/Components/Transform.h>
#include <Editor/Components/Renderable.h>

#include <Editor/Interface/Inspector.h>
#include <Editor/Interface/Outline.h>

#include <Vendor/ImGui/imgui.h>

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

		Scene* scene = SceneManager::GetActiveScene();

		if (scene)
		{
			Actor* actor = InterfaceManager::GetOutline()->GetSelectedActor();
		
			if (actor)
			{
				bool sceneDirty = false;
				bool transformDirty = false;

				if (Transform* transform = actor->GetComponent<Transform>())
				{
					ImGui::PushID(transform);
					if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::PushItemWidth(ImGui::GetWindowWidth() - ImGui::GetTreeNodeToLabelSpacing() - 80);

						R32 r32Max = std::numeric_limits<R32>::max();

						R32V3 localPosition = transform->GetLocalPosition();
						if (ImGui::DragFloat3("Position", &localPosition[0], 0.1F, -r32Max, r32Max, "%.3F", 1.0F))
						{
							transform->SetLocalPosition(localPosition);

							sceneDirty = true;
							transformDirty = true;
						}

						R32V3 eulerAngles = transform->GetLocalEulerAngles();
						if (ImGui::DragFloat3("Rotation", &eulerAngles[0], 0.1F, -r32Max, r32Max, "%.3F", 1.0F))
						{
							transform->SetLocalRotation(eulerAngles);

							sceneDirty = true;
							transformDirty = true;
						}

						R32V3 localScale = transform->GetLocalScale();
						if (ImGui::DragFloat3("Scale", &localScale[0], 0.1F, -r32Max, r32Max, "%.3F", 1.0F))
						{
							transform->SetLocalScale(localScale);

							sceneDirty = true;
							transformDirty = true;
						}

						if (transformDirty)
						{
							Renderable* renderable = actor->GetComponent<Renderable>();

							if (renderable)
							{
								actor->SetAABB(Math::ComputeBoundingBox(renderable->GetVertexBuffer(), transform->GetLocalScale()));
							}
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

							scene->Update();
							scene->Render();
						}
		
						R32 near = camera->GetNear();
						if (ImGui::DragFloat("Near", &near, 0.1F, 0.1F, 100.0F, "%.3F"))
						{
							camera->SetNear(near);

							scene->Update();
							scene->Render();
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
					ImGui::PushID(renderable);

					ImGui::Text("Vertices: %U", (U32)renderable->GetVertexBuffer().size());
					ImGui::Text("Indices: %U", (U32)renderable->GetElementBuffer().size());
					ImGui::Text("Texture Index: %U", renderable->GetTextureIndex());

					ImGui::PopID();
				}

				if (sceneDirty)
				{
					scene->Update();
					scene->Render();
				}
			}
		}

		ImGui::End();
	}
}
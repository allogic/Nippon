#include <Editor/Actor.h>
#include <Editor/Scene.h>
#include <Editor/SceneManager.h>

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

		Scene* scene = SceneManager::GetActiveScene();

		if (scene)
		{
			for (auto& actor : scene->GetActors())
			{
				if (actor->IsRoot())
				{
					DrawActorRecursive(scene, actor);
				}
			}
		}

		ImGui::End();
	}

	void Outline::DrawActorRecursive(Scene* Scene, Actor* Actor)
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

			Scene->Update();
			Scene->Render();
		}

		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 15.0F);
		if (ImGui::Button("x", ImVec2{ 15.0F, 12.0F }))
		{
			Scene->Update();
			Scene->Render();
		}
		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 35.0F);
		if (ImGui::Button("-", ImVec2{ 15.0F, 12.0F }))
		{
			Actor->MakeActiveRecursive(!Actor->IsActive());

			Scene->Update();
			Scene->Render();
		}

		if (opened)
		{
			for (auto& child : Actor->GetChildren())
			{
				DrawActorRecursive(Scene, child);
			}

			ImGui::TreePop();
		}

		ImGui::PopID();
	}
}
#include <Common/Macros.h>

#include <Editor/Actor.h>
#include <Editor/Scene.h>
#include <Editor/SceneManager.h>

#include <Editor/Components/Transform.h>

#include <Editor/Exporter/WavefrontExporter.h>

#include <Editor/Interface/Outline.h>

#include <Editor/ImGui/imgui.h>

namespace ark
{
	void Outline::Reset()
	{
		mSelectedActor = nullptr;
	}

	void Outline::Render()
	{
		ImGui::Begin("Outline");

		if (Scene* scene = SceneManager::GetActiveScene())
		{
			if (Actor* actor = scene->GetRootActor())
			{
				DrawActorRecursive(scene, actor);
			}
		}

		ImGui::End();
	}

	void Outline::SetSelectedActor(Actor* Actor)
	{
		if (Actor)
		{
			Actor->MakeOpenedRecursiveUp(true);
		}

		mSelectedActor = Actor;
	}

	void Outline::DrawActorRecursive(Scene* Scene, Actor* Actor)
	{
		ImGui::PushID(Actor);

		U32 flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		if (Actor == mSelectedActor) flags |= ImGuiTreeNodeFlags_Selected;
		if (Actor->IsOpened()) flags |= ImGuiTreeNodeFlags_DefaultOpen;
		if (Actor->IsChild()) flags |= ImGuiTreeNodeFlags_Leaf;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0F, 5.0F));

		if (Actor->IsActive()) ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_Text]);
		else                   ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);

		U32 opened = ImGui::TreeNodeEx(Actor->GetName().c_str(), flags);

		if (ImGui::IsItemClicked(0))
		{
			mSelectedActor = Actor;

			Scene->Step();
		}

		if (ImGui::BeginPopupContextItem("Outline Context Menu"))
		{
			if (ImGui::MenuItem("Export as Wavefront"))
			{
				WavefrontExporter::Export(mSelectedActor, Scene);
			}
		
			ImGui::EndPopup();
		}

		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 25.0F);
		if (ImGui::Button("x", ImVec2{ 25.0F, 0.0F }))
		{
			if (Actor == mSelectedActor)
			{
				mSelectedActor = nullptr;
			}

			Actor->MakeShouldBeDestroyedRecursiveDown(true);

			Scene->Step();
		}
		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 50.0F);
		if (ImGui::Button("-", ImVec2{ 25.0F, 0.0F }))
		{
			Actor->MakeActiveRecursiveDown(!Actor->IsActive());

			Scene->Step();
		}

		ImGui::PopStyleColor();

		if (opened)
		{
			for (auto& child : Actor->GetChildren())
			{
				DrawActorRecursive(Scene, child);
			}

			ImGui::TreePop();
		}
		else
		{
			Actor->MakeOpenedRecursiveDown(false); // TODO
		}

		ImGui::PopStyleVar();

		ImGui::PopID();
	}
}

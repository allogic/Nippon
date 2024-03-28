#include <Ecs/Registry.h>
#include <Ecs/Entity.h>

#include <Export/WavefrontExporter.h>

#include <Font/MaterialDesignIcons.h>

#include <Interface/Outline.h>

#include <ImGui/imgui.h>

#include <Scene/Scene.h>
#include <Scene/SceneManager.h>

namespace Nippon
{
	static Entity* sSelectedEntity = nullptr;

	void Outline::Reset()
	{
		sSelectedEntity = nullptr;
	}

	void Outline::Render()
	{
		ImGui::Begin(ICON_MDI_FILE_TREE " Outline");

		if (Scene* scene = SceneManager::GetCurrentScene())
		{
			if (Registry* registry = scene->GetRegistry())
			{
				if (Entity* entity = registry->GetRootEntity())

				DrawEntityTreeRecursive(scene, entity);
			}
		}

		ImGui::End();
	}

	Entity* Outline::GetSelectedEntity()
	{
		return sSelectedEntity;
	}

	void Outline::SetSelectedEntity(Entity* Entity)
	{
		if (Entity)
		{
			Entity->SetOpenedUp(true);
		}

		sSelectedEntity = Entity;
	}

	bool Outline::DrawEntityTreeRecursive(Scene* Scene, Entity* Entity)
	{
		bool dirty = false;

		ImGui::PushID(Entity);

		U32 flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		if (Entity == sSelectedEntity) flags |= ImGuiTreeNodeFlags_Selected;
		if (Entity->IsOpened())        flags |= ImGuiTreeNodeFlags_DefaultOpen;
		if (Entity->IsChild())         flags |= ImGuiTreeNodeFlags_Leaf;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 5.0F, 5.0F });

		if (Entity->IsActive()) ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_Text]);
		else                    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);

		U32 opened = ImGui::TreeNodeEx(Entity->GetName().data(), flags);

		if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1))
		{
			sSelectedEntity = Entity;

			Scene->Invalidate();
		}

		if (ImGui::BeginPopupContextItem("Outline Context Menu"))
		{
			if (ImGui::MenuItem(ICON_MDI_PLUS " Add"))
			{
				if (Registry* registry = Scene->GetRegistry())
				{
					registry->CreateEntity("", sSelectedEntity);

					dirty = true;
				}
			}

			if (ImGui::MenuItem(ICON_MDI_MINUS " Remove"))
			{
				if (sSelectedEntity)
				{
					sSelectedEntity->SetShouldBeDestroyed();

					Scene->Invalidate();

					dirty = true;
				}
			}

			if (ImGui::MenuItem(ICON_MDI_MINUS " Remove Recursive"))
			{
				if (sSelectedEntity)
				{
					sSelectedEntity->SetShouldBeDestroyedDown();

					Scene->Invalidate();

					dirty = true;
				}
			}

			ImGui::Separator();

			if (ImGui::MenuItem(ICON_MDI_RENAME " Rename"))
			{
				// TODO
			}

			ImGui::Separator();

			if (ImGui::MenuItem(ICON_MDI_EXPORT " Export as Wavefront"))
			{
				if (sSelectedEntity)
				{
					WavefrontExporter::Export(sSelectedEntity, Scene);
				}
			}

			ImGui::EndPopup();
		}

		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 25.0F);
		if (ImGui::Button(ICON_MDI_CLOSE_THICK, ImVec2{ 25.0F, 0.0F }))
		{
			Entity->SetShouldBeDestroyedDown();

			Scene->Invalidate();
			
		}
		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 50.0F);
		if (ImGui::Button(Entity->IsActive() ? ICON_MDI_EYE_OUTLINE : ICON_MDI_EYE_OFF_OUTLINE, ImVec2{ 25.0F, 0.0F }))
		{
			Entity->SetActiveDown(!Entity->IsActive());

			Scene->Invalidate();
		}

		ImGui::PopStyleColor();

		if (opened)
		{
			if (!dirty)
			{
				for (auto& child : Entity->GetChildren())
				{
					dirty = DrawEntityTreeRecursive(Scene, child);

					if (dirty)
					{
						break;
					}
				}
			}

			ImGui::TreePop();
		}
		else
		{
			//Entity->SetOpenedDown(false); // TODO
		}

		ImGui::PopStyleVar();

		ImGui::PopID();

		return dirty;
	}
}
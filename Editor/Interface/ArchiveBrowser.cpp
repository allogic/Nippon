#include <Editor/Database/Database.h>

#include <Editor/Font/MaterialDesignIcons.h>

#include <Editor/Interface/ArchiveBrowser.h>

#include <Editor/ImGui/imgui.h>

#include <Editor/Scene/Scene.h>
#include <Editor/Scene/SceneManager.h>

namespace Nippon
{
	static std::string sSelectedFolderId = "";

	void ArchiveBrowser::Reset()
	{
		sSelectedFolderId = "";
	}

	void ArchiveBrowser::Render()
	{
		ImGui::Begin(ICON_MDI_ARCHIVE " Archive Browser");

		if (Database::IsReady())
		{
			ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

			ImGui::BeginChild("Archive Folders", ImVec2{ 200.0F, contentRegionAvail.y });

			auto const& folderInfos = Database::GetFolderInfos();

			for (auto const& folderInfo : folderInfos)
			{
				ImGui::PushID(&folderInfo);
			
				if (sSelectedFolderId == folderInfo.FolderId)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));

					if (ImGui::Button(folderInfo.GetFolderName().data(), ImVec2{-1.0F, 30.0F}))
					{
						sSelectedFolderId = folderInfo.FolderId;
					}

					ImGui::PopStyleColor();
				}
				else
				{
					if (ImGui::Button(folderInfo.GetFolderName().data(), ImVec2{-1.0F, 30.0F}))
					{
						sSelectedFolderId = folderInfo.FolderId;
					}
				}

				ImGui::PopID();
			}

			ImGui::EndChild();

			ImGui::SameLine();

			ImGui::BeginChild("Archive Thumbnails", ImVec2{ contentRegionAvail.x - 200.0F, contentRegionAvail.y });

			ImGuiStyle& imGuiStyle = ImGui::GetStyle();

			auto const& archiveInfos = Database::GetArchiveInfosByFolderId(sSelectedFolderId);

			U64 buttonCount = archiveInfos.size();
			R32 windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

			for (U32 i = 0; i < buttonCount; i++)
			{
				ImGui::PushID(&archiveInfos[i]);

				U32 thumbnailTexture = Database::GetThumbnailTextureByArchiveId(archiveInfos[i].UniqueId);

				if (ImGui::ImageButton((ImTextureID)(U64)thumbnailTexture, ImVec2{ ARCHIVE_THUMBNAIL_SIZE, ARCHIVE_THUMBNAIL_SIZE }))
				{
					if (Scene* scene = SceneManager::CreateScene(archiveInfos[i]))
					{
						scene->CreateViewport();
						scene->SetDebugEnabled();
						scene->Load();
						scene->Invalidate();
					}
				}

				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text(archiveInfos[i].WindowName.data());
					ImGui::EndTooltip();
				}

				R32 lastButtonX2 = ImGui::GetItemRectMax().x;
				R32 nextButtonX2 = lastButtonX2 + imGuiStyle.ItemSpacing.x + imGuiStyle.ScrollbarSize + ARCHIVE_THUMBNAIL_SIZE;

				if ((i + 1) < buttonCount && (nextButtonX2 < windowVisibleX2))
				{
					ImGui::SameLine();
				}

				ImGui::PopID();
			}

			ImGui::EndChild();
		}

		ImGui::End();
	}
}
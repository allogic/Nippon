#pragma once

#include <Common/Forward.h>
#include <Common/Standard.h>
#include <Common/Types.h>

#include <Editor/Forward.h>

#include <Editor/Database/Database.h>

#include <Editor/ImGui/imgui.h>
#include <Editor/ImGui/imgui_internal.h>

namespace Nippon
{
	class MainMenu
	{
	public:

		static void Reset();
		static void Render();

	private:

		static void RenderArchiveMenu();
		static void RenderDatabaseMenu();
		static void RenderToolMenu();
		static void RenderSettingsMenu();

	private:

		static void RenderConfigurationPopupDialog();

	private:

		static void OpenSceneProcedure(ArchiveInfo const& ArchiveInfo);
		static void PrintTableOfContentProcedure(ArchiveInfo const& ArchiveInfo);
		static void ExtractToDiskProcedure(ArchiveInfo const& ArchiveInfo);

	private:

		template<typename P, typename ... Args>
		static void RenderMenuWithProcedure(P Procedure, Args&& ... Arguments);
	};
}

namespace Nippon
{
	template<typename P, typename ... Args>
	void MainMenu::RenderMenuWithProcedure(P Procedure, Args&& ... Arguments)
	{
		if (ImGui::MenuItem("All", "", nullptr))
		{
			for (auto const& archiveInfo : Database::GetAllArchiveInfos())
			{
				Procedure(archiveInfo, std::forward<Args>(Arguments) ...);
			}
		}
		
		ImGui::Separator();
		
		if (ImGui::BeginMenu("Levels"))
		{
			if (ImGui::MenuItem("All", "", nullptr))
			{
				for (auto const& archiveInfo : Database::GetLevelArchiveInfos())
				{
					Procedure(archiveInfo, std::forward<Args>(Arguments) ...);
				}
			}
		
			ImGui::Separator();
		
			for (auto const& folderInfo : Database::GetLevelFolderInfos())
			{
				ImGui::PushID(&folderInfo);
		
				if (ImGui::BeginMenu(folderInfo.GetFolderName().data()))
				{
					if (ImGui::MenuItem("All", "", nullptr))
					{
						for (auto const& archiveInfo : Database::GetArchiveInfosByFolderId(folderInfo.FolderId))
						{
							Procedure(archiveInfo, std::forward<Args>(Arguments) ...);
						}
					}
		
					ImGui::Separator();
		
					for (auto const& archiveInfo : Database::GetArchiveInfosByFolderId(folderInfo.FolderId))
					{
						ImGui::PushID(&archiveInfo);
					
						if (ImGui::MenuItem(archiveInfo.GetArchiveName().data(), "", nullptr))
						{
							Procedure(archiveInfo, std::forward<Args>(Arguments) ...);
						}
					
						ImGui::PopID();
					}
		
					ImGui::EndMenu();
				}
		
				ImGui::PopID();
			}
		
			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu("Entities"))
		{
			if (ImGui::MenuItem("All", "", nullptr))
			{
				for (auto const& archiveInfo : Database::GetEntityArchiveInfos())
				{
					Procedure(archiveInfo, std::forward<Args>(Arguments) ...);
				}
			}
		
			ImGui::Separator();
		
			for (auto const& folderInfo : Database::GetEntityFolderInfos())
			{
				ImGui::PushID(&folderInfo);
		
				if (ImGui::BeginMenu(folderInfo.GetFolderName().data()))
				{
					if (ImGui::MenuItem("All", "", nullptr))
					{
						for (auto const& archiveInfo : Database::GetArchiveInfosByFolderId(folderInfo.FolderId))
						{
							Procedure(archiveInfo, std::forward<Args>(Arguments) ...);
						}
					}
		
					ImGui::Separator();
		
					for (auto const& archiveInfo : Database::GetArchiveInfosByFolderId(folderInfo.FolderId))
					{
						ImGui::PushID(&archiveInfo);
					
						if (ImGui::MenuItem(archiveInfo.GetArchiveName().data(), "", nullptr))
						{
							Procedure(archiveInfo, std::forward<Args>(Arguments) ...);
						}
					
						ImGui::PopID();
					}
		
					ImGui::EndMenu();
				}
		
				ImGui::PopID();
			}
		
			ImGui::EndMenu();
		}
	}
}
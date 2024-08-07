#include <Common/Archive/Archive.h>

#include <Common/Utility/FileUtility.h>

#include <Editor/Font/MaterialDesignIcons.h>

#include <Editor/ImGui/FileDialog/ImGuiFileDialog.h>

#include <Editor/Interface/MainMenu.h>
#include <Editor/Interface/Log.h>

#include <Editor/Scene/Scene.h>
#include <Editor/Scene/SceneManager.h>

namespace Nippon
{
	static char const* sConfigurationPopupName = "Database Configuration";
	static ImGuiID sConfigurationPopupId = 0;

	void MainMenu::Reset()
	{
		sConfigurationPopupId = ImHashStr(sConfigurationPopupName);
	}

	void MainMenu::Render()
	{
		ImGui::BeginMainMenuBar();

		RenderFileMenu();
		RenderDatabaseMenu();
		RenderToolMenu();

		ImGui::EndMainMenuBar();

		static bool initialized = false;

		if (!initialized && !Database::IsFound())
		{
			initialized = true;

			ImGui::PushOverrideID(sConfigurationPopupId);
			ImGui::OpenPopup(sConfigurationPopupName);
			ImGui::PopID();
		}

		RenderConfigurationPopupDialog();
		RenderFileDialog();
	}

	void MainMenu::RenderFileMenu()
	{
		if (ImGui::BeginMenu(ICON_MDI_FILE " File"))
		{
			if (ImGui::BeginMenu(ICON_MDI_FOLDER_OPEN " Open"))
			{
				if (ImGui::MenuItem("Level Archive", "", nullptr))
				{
					IGFD::FileDialogConfig config = {};

					config.path = ".";
					config.flags = 0;

					ImGuiFileDialog::Instance()->OpenDialog("OpenLevelArchiveFileDlg", "Choose Level Archive", ".dat", config);
				}

				if (ImGui::MenuItem("Entity Archive", "", nullptr))
				{
					IGFD::FileDialogConfig config = {};

					config.path = ".";
					config.flags = 0;

					ImGuiFileDialog::Instance()->OpenDialog("OpenEntityArchiveFileDlg", "Choose Entity Archive", ".dat", config);
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}
	}

	void MainMenu::RenderDatabaseMenu()
	{
		if (ImGui::BeginMenu(ICON_MDI_DATABASE " Database"))
		{
			if (ImGui::BeginMenu(ICON_MDI_ARCHIVE " Archive", Database::IsReady()))
			{
				if (ImGui::BeginMenu(ICON_MDI_FOLDER_OPEN " Open"))
				{
					RenderMenuWithProcedure(OpenSceneProcedure);

					ImGui::EndMenu();
				}

				ImGui::Separator();

				if (ImGui::BeginMenu(ICON_MDI_TABLE_OF_CONTENTS " Print Table Of Content"))
				{
					RenderMenuWithProcedure(PrintTableOfContentProcedure);

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu(ICON_MDI_FILE_SEND " Extract To Disk"))
				{
					RenderMenuWithProcedure(ExtractToDiskProcedure);

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu(ICON_MDI_FILE_SEND " Unfold To Disk"))
				{
					RenderMenuWithProcedure(UnfoldToDiskProcedure);

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::MenuItem(ICON_MDI_DATABASE_SYNC " Rebuild Full", "", nullptr, !Database::IsBusy()))
			{
				Database::RebuildFull();
			}

			if (ImGui::MenuItem(ICON_MDI_DATABASE_SYNC " Rebuild Archives", "", nullptr, !Database::IsBusy()))
			{
				Database::RebuildArchives();
			}

			if (ImGui::MenuItem(ICON_MDI_DATABASE_SYNC " Rebuild Thumbnails", "", nullptr, !Database::IsBusy() && Database::IsReady()))
			{
				Database::RebuildThumbnails();
			}

			if (ImGui::MenuItem(ICON_MDI_DATABASE_REMOVE " Cancel Rebuild", "", nullptr, Database::IsBusy()))
			{
				Database::SetCancelJob();
			}

			ImGui::Separator();

			if (ImGui::MenuItem(ICON_MDI_DATABASE_SETTINGS " Settings", "", nullptr, Database::IsReady()))
			{
				ImGui::PushOverrideID(sConfigurationPopupId);
				ImGui::OpenPopup(sConfigurationPopupName);
				ImGui::PopID();
			}

			ImGui::EndMenu();
		}
	}

	void MainMenu::RenderToolMenu()
	{
		if (ImGui::BeginMenu(ICON_MDI_TOOLS " Tools", Database::IsReady()))
		{
			if (ImGui::MenuItem(ICON_MDI_FILE_SEND " Generate imgui.ini", "", nullptr))
			{
				Database::GenerateImGuiIni();
			}

			if (ImGui::MenuItem(ICON_MDI_FILE_SEND " Generate ModelInfo.h", "", nullptr))
			{
				Database::GenerateModelInfoHeader();
			}

			ImGui::EndMenu();
		}
	}

	void MainMenu::RenderConfigurationPopupDialog()
	{
		static bool initialized = false;
		static char dataDir[1024] = "";
		static Configuration configuration = {};

		U32 flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;

		ImGui::PushOverrideID(sConfigurationPopupId);
		if (ImGui::BeginPopupModal(sConfigurationPopupName, nullptr, flags))
		{
			if (!initialized)
			{
				initialized = true;

				configuration = Database::GetConfiguration();

				std::memcpy(dataDir, configuration.DataDir.string().data(), glm::min(configuration.DataDir.string().size(), sizeof(dataDir)));
			}

			ImGui::InputText("Data Directory", dataDir, sizeof(dataDir));

			if (ImGui::Button("Save"))
			{
				initialized = false;

				configuration.DataDir = dataDir;

				Database::SetConfiguration(configuration);

				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Close"))
			{
				initialized = false;

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
		ImGui::PopID();
	}

	void MainMenu::RenderFileDialog()
	{
		if (ImGuiFileDialog::Instance()->Display("OpenLevelArchiveFileDlg"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				fs::path filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

				filePath /= ImGuiFileDialog::Instance()->GetCurrentFileName();

				if (Scene* scene = SceneManager::CreateSceneFromFile(filePath, eArchiveTypeLevel))
				{
					scene->CreateViewport();
					scene->SetDebugEnabled();
					scene->Load();
					scene->Invalidate();
				}
			}

			ImGuiFileDialog::Instance()->Close();
		}

		if (ImGuiFileDialog::Instance()->Display("OpenEntityArchiveFileDlg"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				fs::path filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

				filePath /= ImGuiFileDialog::Instance()->GetCurrentFileName();

				if (Scene* scene = SceneManager::CreateSceneFromFile(filePath, eArchiveTypeEntity))
				{
					scene->CreateViewport();
					scene->SetDebugEnabled();
					scene->Load();
					scene->Invalidate();
				}
			}

			ImGuiFileDialog::Instance()->Close();
		}
	}

	void MainMenu::OpenSceneProcedure(ArchiveInfo const& ArchiveInfo)
	{
		if (Scene* scene = SceneManager::CreateSceneFromDatabase(ArchiveInfo))
		{
			scene->CreateViewport();
			scene->SetDebugEnabled();
			scene->Load();
			scene->Invalidate();
		}
	}

	void MainMenu::PrintTableOfContentProcedure(ArchiveInfo const& ArchiveInfo)
	{
		std::vector<U8> const& archiveData = Database::GetArchiveDataByUniqueId(ArchiveInfo.UniqueId);
		
		Archive archive = {};
		
		archive.Deserialize(archiveData);
		archive.PrintTableOfContent([](char const* Message) { Log::Add(Message); });
	}

	void MainMenu::ExtractToDiskProcedure(ArchiveInfo const& ArchiveInfo)
	{
		std::vector<U8> const& archiveData = Database::GetArchiveDataByUniqueId(ArchiveInfo.UniqueId);

		fs::path outputFile = fs::path{ "Extracted" } / ArchiveInfo.FolderId / ArchiveInfo.ArchiveId;

		FileUtility::CreateDirIfNotExist(outputFile);

		Archive archive = {};

		archive.Deserialize(archiveData);
		archive.ExtractToDisk(outputFile);
	}

	void MainMenu::UnfoldToDiskProcedure(ArchiveInfo const& ArchiveInfo)
	{
		std::vector<U8> const& archiveData = Database::GetArchiveDataByUniqueId(ArchiveInfo.UniqueId);

		fs::path outputFile = fs::path{ "Unfolded" } / ArchiveInfo.FolderId / ArchiveInfo.ArchiveId;

		FileUtility::CreateDirIfNotExist(outputFile);

		Archive archive = {};

		archive.Deserialize(archiveData);
		archive.UnfoldToDisk(outputFile);
	}
}
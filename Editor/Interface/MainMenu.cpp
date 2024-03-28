#include <Archive/Archive.h>

#include <Font/MaterialDesignIcons.h>

#include <Interface/MainMenu.h>

#include <Scene/Scene.h>
#include <Scene/SceneManager.h>

#include <Utility/FileUtility.h>

namespace Nippon
{
	static char const* sConfigurationPopupName = "Editor Configuration";
	static ImGuiID sConfigurationPopupId = 0;

	void MainMenu::Reset()
	{
		sConfigurationPopupId = ImHashStr(sConfigurationPopupName);
	}

	void MainMenu::Render()
	{
		ImGui::BeginMainMenuBar();

		RenderArchiveMenu();
		RenderDatabaseMenu();
		RenderToolMenu();
		RenderSettingsMenu();

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
	}

	void MainMenu::RenderArchiveMenu()
	{
		if (ImGui::BeginMenu(ICON_MDI_ARCHIVE " Archive", Database::IsReady()))
		{
			if (ImGui::BeginMenu("Open Read Only"))
			{
				RenderMenuWithProcedure(OpenSceneProcedure);

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("Table Of Content"))
			{
				RenderMenuWithProcedure(PrintTableOfContentProcedure);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Extract To Disk"))
			{
				RenderMenuWithProcedure(ExtractToDiskProcedure);

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}
	}

	void MainMenu::RenderDatabaseMenu()
	{
		if (ImGui::BeginMenu(ICON_MDI_DATABASE " Database"))
		{
			if (ImGui::MenuItem("Rebuild Full", "", nullptr, !Database::IsBusy()))
			{
				Database::RebuildFull();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Rebuild Archives", "", nullptr, !Database::IsBusy()))
			{
				Database::RebuildArchives();
			}

			if (ImGui::MenuItem("Rebuild Thumbnails", "", nullptr, !Database::IsBusy() && Database::IsReady()))
			{
				Database::RebuildThumbnails();
			}

			if (ImGui::MenuItem("Cancel Rebuild", "", nullptr, Database::IsBusy()))
			{
				Database::SetCancelJob();
			}

			ImGui::EndMenu();
		}
	}

	void MainMenu::RenderToolMenu()
	{
		if (ImGui::BeginMenu(ICON_MDI_TOOLS " Tool", Database::IsReady()))
		{
			if (ImGui::MenuItem("Generate ImGui Ini", "", nullptr))
			{
				Database::GenerateImGuiIni();
			}

			ImGui::EndMenu();
		}
	}

	void MainMenu::RenderSettingsMenu()
	{
		if (ImGui::BeginMenu(ICON_MDI_COG " Settings"))
		{
			if (ImGui::MenuItem("Editor Configuration", "", nullptr))
			{
				ImGui::PushOverrideID(sConfigurationPopupId);
				ImGui::OpenPopup(sConfigurationPopupName);
				ImGui::PopID();
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

	void MainMenu::OpenSceneProcedure(ArchiveInfo const& ArchiveInfo)
	{
		if (Scene* scene = SceneManager::CreateScene(ArchiveInfo))
		{
			scene->CreateViewport();
			scene->SetDebugEnabled();
			scene->Load();
			scene->Invalidate();
		}
	}

	void MainMenu::PrintTableOfContentProcedure(ArchiveInfo const& ArchiveInfo)
	{
		std::vector<U8> archiveData = Database::GetArchiveDataByUniqueId(ArchiveInfo.UniqueId);
		
		Archive archive = {};
		
		archive.DeSerialize(archiveData);
		archive.PrintTableOfContent();
	}

	void MainMenu::ExtractToDiskProcedure(ArchiveInfo const& ArchiveInfo)
	{
		std::vector<U8> archiveData = Database::GetArchiveDataByUniqueId(ArchiveInfo.UniqueId);

		fs::path outputFile = fs::path{ "Extracted" } / ArchiveInfo.FolderId / ArchiveInfo.ArchiveId;

		FileUtility::CreateDirIfNotExist(outputFile);

		Archive archive = {};

		archive.DeSerialize(archiveData);
		archive.ExtractToDisk(outputFile);
	}
}
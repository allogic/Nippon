#include <Common/Macros.h>
#include <Common/Archive.h>
#include <Common/BlowFish.h>

#include <Common/Utilities/DiffUtils.h>
#include <Common/Utilities/FsUtils.h>

#include <Editor/Editor.h>
#include <Editor/Scene.h>
#include <Editor/SceneManager.h>

#include <Editor/Databases/FileDatabase.h>
#include <Editor/Databases/ThumbnailDatabase.h>

#include <Editor/Interface/MainMenu.h>

namespace ark
{
	MainMenu::MainMenu()
	{

	}

	MainMenu::~MainMenu()
	{

	}

	void MainMenu::Reset()
	{

	}

	void MainMenu::Render()
	{
		ImGui::BeginMainMenuBar();

		RenderSceneMenu();
		RenderArchiveMenu();
		RenderToolsMenu();

		ImGui::EndMainMenuBar();
	}

	void MainMenu::RenderSceneMenu()
	{
		if (ImGui::BeginMenu("Scene"))
		{
			if (ImGui::BeginMenu("Open"))
			{
				RenderMenuWithProcedure(OpenSceneProcedure);

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}
	}

	void MainMenu::RenderArchiveMenu()
	{
		if (ImGui::BeginMenu("Archive"))
		{
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

	void MainMenu::RenderToolsMenu()
	{
		if (ImGui::BeginMenu("Tools"))
		{
			if (ImGui::Selectable("Generate File Database"))
			{
				FileDatabase::Generate();
			}

			if (ImGui::Selectable("Generate Thumbnail Database"))
			{
				ThumbnailDatabase::Generate();
			}

			ImGui::Separator();

			if (ImGui::Selectable("Generate ImGui Config"))
			{
				GenerateImGuiConfig();
			}

			ImGui::EndMenu();
		}
	}

	void MainMenu::GenerateImGuiConfig()
	{
		std::ostringstream oss = {};

		for (const auto& directory : FileDatabase::GetDirectories())
		{
			for (const auto& fileContainer : FileDatabase::GetFileContainersByDirectory(directory))
			{
				oss << "[Window][" << fileContainer->GetWindowName() << "]\n";
				oss << "Pos=308,28\n";
				oss << "Size=1044,1052\n";
				oss << "Collapsed=0\n";
				oss << "DockId=0x00000002,0\n";
				oss << "\n";
			}
		}

		FsUtils::WriteText("imgui_viewports.ini", oss.str().c_str());
	}

	void MainMenu::OpenSceneProcedure(const FileContainer* FileContainer)
	{
		if (Scene* scene = SceneManager::CreateScene(FileContainer))
		{
			scene->SetEnableDebug(true);

			scene->CreateViewport();
			scene->Load();
			scene->Invalidate();
		}
	}

	void MainMenu::PrintTableOfContentProcedure(const FileContainer* FileContainer)
	{
		fs::path inputDatFile = gDataDir / FileContainer->GetDatFile()->GetRelativeFile();
		fs::path inputBinFile = gDataDir / FileContainer->GetBinFile()->GetRelativeFile();

		std::vector<U8> datBytes = FsUtils::ReadBinary(inputDatFile);

		gBlowFish->Decrypt(datBytes);

		Archive datArchive = {};

		datArchive.DeSerialize(datBytes);
		datArchive.PrintTableOfContent();

		if (fs::exists(inputBinFile) && fs::is_regular_file(inputBinFile))
		{
			std::vector<U8> binBytes = FsUtils::ReadBinary(inputBinFile);

			gBlowFish->Decrypt(binBytes);

			Archive binArchive = {};

			binArchive.DeSerialize(binBytes);
			binArchive.PrintTableOfContent();
		}
	}

	void MainMenu::ExtractToDiskProcedure(const FileContainer* FileContainer)
	{
		fs::path inputDatFile = gDataDir / FileContainer->GetDatFile()->GetRelativeFile();
		fs::path inputBinFile = gDataDir / FileContainer->GetBinFile()->GetRelativeFile();

		fs::path outputDatFile = fs::path{ "Extracted" } / FileContainer->GetDatFile()->GetRelativeFile();
		fs::path outputBinFile = fs::path{ "Extracted" } / FileContainer->GetBinFile()->GetRelativeFile();

		FsUtils::CreateDirIfNotExist(outputDatFile, true);

		std::vector<U8> datBytes = FsUtils::ReadBinary(inputDatFile);

		gBlowFish->Decrypt(datBytes);

		Archive datArchive = {};

		datArchive.DeSerialize(datBytes);
		datArchive.ExtractToDisk(outputDatFile);

		if (fs::exists(inputBinFile) && fs::is_regular_file(inputBinFile))
		{
			FsUtils::CreateDirIfNotExist(outputBinFile, true);

			std::vector<U8> binBytes = FsUtils::ReadBinary(inputBinFile);

			gBlowFish->Decrypt(binBytes);

			Archive binArchive = {};

			binArchive.DeSerialize(binBytes);
			binArchive.ExtractToDisk(outputBinFile);
		}
	}
}

#include <Common/Macros.h>
#include <Common/Archive.h>
#include <Common/BlowFish.h>

#include <Common/Utilities/DiffUtils.h>
#include <Common/Utilities/FsUtils.h>

#include <Editor/Editor.h>
#include <Editor/Scene.h>
#include <Editor/SceneManager.h>
#include <Editor/Thumbnail.h>
#include <Editor/TextureLoader.h>

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

		RenderFileMenu();
		RenderEditMenu();
		RenderSceneMenu();
		RenderArchiveMenu();
		RenderToolsMenu();

		ImGui::EndMainMenuBar();
	}

	void MainMenu::RenderFileMenu()
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::EndMenu();
		}
	}

	void MainMenu::RenderEditMenu()
	{
		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::EndMenu();
		}
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
			if (ImGui::Selectable("Test Kamui"))
			{
				ExecuteLevelByGroupKeyProcedure("st3", CopyAndDecryptOriginalToScratchProcedure);
				ExecuteLevelByGroupKeyProcedure("st3", LoadAndSaveFromScratchToScratchProcedure);
				ExecuteLevelByGroupKeyProcedure("st3", CopyScratchToFinishedProcedure);
				ExecuteLevelByGroupKeyProcedure("st3", CopyAndDecryptOriginalToScratchProcedure);
				ExecuteLevelByGroupKeyProcedure("st3", CompareScratchWithFinishedProcedure);
			}

			if (ImGui::Selectable("Test Humans"))
			{
				ExecuteEntityByGroupKeyProcedure("hm", CopyAndDecryptOriginalToScratchProcedure);
				ExecuteEntityByGroupKeyProcedure("hm", LoadAndSaveFromScratchToScratchProcedure);
				ExecuteEntityByGroupKeyProcedure("hm", CopyScratchToFinishedProcedure);
				ExecuteEntityByGroupKeyProcedure("hm", CopyAndDecryptOriginalToScratchProcedure);
				ExecuteEntityByGroupKeyProcedure("hm", CompareScratchWithFinishedProcedure);
			}

			if (ImGui::BeginMenu("Copy And Decrypt Original To Scratch"))
			{
				RenderMenuWithProcedure(CopyAndDecryptOriginalToScratchProcedure);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Load And Save From Scratch To Scratch (Debug Only)"))
			{
				RenderMenuWithProcedure(LoadAndSaveFromScratchToScratchProcedure);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Encrypt And Copy Scratch To Finished"))
			{
				RenderMenuWithProcedure(EncryptAndCopyScratchToFinishedProcedure);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Copy Scratch To Finished"))
			{
				RenderMenuWithProcedure(CopyScratchToFinishedProcedure);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Compare Scratch With Finished (Debug Only)"))
			{
				RenderMenuWithProcedure(CompareScratchWithFinishedProcedure);

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

	void MainMenu::RenderToolsMenu()
	{
		if (ImGui::BeginMenu("Tools"))
		{
			if (ImGui::BeginMenu("Generate Thumbnails"))
			{
				RenderMenuWithProcedure(CreateThumbnailProcedure);

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}
	}

	void MainMenu::DebugProcedure(const SceneInfo* SceneInfo)
	{
		LOG("%s %s\n", SceneInfo->GroupKey.c_str(), SceneInfo->SceneKey.c_str());
	}

	void MainMenu::OpenSceneProcedure(const SceneInfo* SceneInfo)
	{
		if (Scene* scene = SceneManager::CreateScene(*SceneInfo))
		{
			scene->SetEnableConsole(true);
			scene->SetEnableDebug(true);

			scene->CreateViewport();
			scene->Load();
		}
	}

	void MainMenu::CopyAndDecryptOriginalToScratchProcedure(const SceneInfo* SceneInfo)
	{
		fs::path inputDatFile = gDataDir / fs::path{ SceneInfo->GroupKey } / SceneInfo->DatArchiveFileName;
		fs::path inputBinFile = gDataDir / fs::path{ SceneInfo->GroupKey } / SceneInfo->BinArchiveFileName;

		fs::path outputDatFile = fs::path{ "Scratch" } / fs::path{ SceneInfo->GroupKey } / SceneInfo->DatArchiveFileName;
		fs::path outputBinFile = fs::path{ "Scratch" } / fs::path{ SceneInfo->GroupKey } / SceneInfo->BinArchiveFileName;

		if (fs::exists(inputDatFile))
		{
			FsUtils::CreateDirIfNotExist(outputDatFile);

			std::vector<U8> datBytes = FsUtils::ReadBinary(inputDatFile);

			gBlowFish->Decrypt(datBytes);

			FsUtils::WriteBinary(outputDatFile, datBytes);

			LOG("Copied and decrypted from %s to %s\n", inputDatFile.relative_path().string().c_str(), outputDatFile.relative_path().string().c_str());
		}
		else
		{
			LOG("Input file %s does not exit\n", inputDatFile.string().c_str());
		}
		
		if (SceneInfo->Type == eSceneTypeLevel)
		{
			if (fs::exists(inputBinFile))
			{
				FsUtils::CreateDirIfNotExist(outputBinFile);

				std::vector<U8> binBytes = FsUtils::ReadBinary(inputBinFile);

				gBlowFish->Decrypt(binBytes);

				FsUtils::WriteBinary(outputBinFile, binBytes);

				LOG("Copied and decrypted from %s to %s\n", inputBinFile.relative_path().string().c_str(), outputBinFile.relative_path().string().c_str());
			}
			else
			{
				LOG("Input file %s does not exit\n", inputBinFile.string().c_str());
			}
		}
	}

	void MainMenu::LoadAndSaveFromScratchToScratchProcedure(const SceneInfo* SceneInfo)
	{
		fs::path inputDatFile = fs::path{ "Scratch" } / SceneInfo->GroupKey / SceneInfo->DatArchiveFileName;
		fs::path inputBinFile = fs::path{ "Scratch" } / SceneInfo->GroupKey / SceneInfo->BinArchiveFileName;

		fs::path outputDatFile = fs::path{ "Scratch" } / SceneInfo->GroupKey / SceneInfo->DatArchiveFileName;
		fs::path outputBinFile = fs::path{ "Scratch" } / SceneInfo->GroupKey / SceneInfo->BinArchiveFileName;

		if (fs::exists(inputDatFile))
		{
			FsUtils::CreateDirIfNotExist(outputDatFile);

			Archive datArchive = {};

			datArchive.DeSerialize(FsUtils::ReadBinary(inputDatFile));

			FsUtils::WriteBinary(outputDatFile, datArchive.Serialize());

			LOG("Loaded and saved from %s to %s\n", inputDatFile.relative_path().string().c_str(), outputDatFile.relative_path().string().c_str());
		}
		else
		{
			LOG("Input file %s does not exit\n", inputDatFile.string().c_str());
		}

		if (SceneInfo->Type == eSceneTypeLevel)
		{
			if (fs::exists(inputBinFile))
			{
				FsUtils::CreateDirIfNotExist(outputBinFile);

				Archive binArchive = {};

				binArchive.DeSerialize(FsUtils::ReadBinary(inputBinFile));

				FsUtils::WriteBinary(outputBinFile, binArchive.Serialize());

				LOG("Loaded and saved from %s to %s\n", inputBinFile.relative_path().string().c_str(), outputBinFile.relative_path().string().c_str());
			}
			else
			{
				LOG("Input file %s does not exit\n", outputBinFile.string().c_str());
			}
		}
	}

	void MainMenu::EncryptAndCopyScratchToFinishedProcedure(const SceneInfo* SceneInfo)
	{
		fs::path inputDatFile = fs::path{ "Scratch" } / SceneInfo->GroupKey / SceneInfo->DatArchiveFileName;
		fs::path inputBinFile = fs::path{ "Scratch" } / SceneInfo->GroupKey / SceneInfo->BinArchiveFileName;

		fs::path outputDatFile = fs::path{ "Finished" } / SceneInfo->GroupKey / SceneInfo->DatArchiveFileName;
		fs::path outputBinFile = fs::path{ "Finished" } / SceneInfo->GroupKey / SceneInfo->BinArchiveFileName;

		if (fs::exists(inputDatFile))
		{
			FsUtils::CreateDirIfNotExist(outputDatFile);

			std::vector<U8> datBytes = FsUtils::ReadBinary(inputDatFile);

			gBlowFish->Encrypt(datBytes);

			FsUtils::WriteBinary(outputDatFile, datBytes);

			LOG("Encrypted and copied from %s to %s\n", inputDatFile.relative_path().string().c_str(), outputDatFile.relative_path().string().c_str());
		}
		else
		{
			LOG("Input file %s does not exit\n", inputDatFile.string().c_str());
		}

		if (SceneInfo->Type == eSceneTypeLevel)
		{
			if (fs::exists(inputBinFile))
			{
				FsUtils::CreateDirIfNotExist(outputBinFile);

				std::vector<U8> binBytes = FsUtils::ReadBinary(inputBinFile);

				gBlowFish->Encrypt(binBytes);

				FsUtils::WriteBinary(outputBinFile, binBytes);

				LOG("Encrypted and copied from %s to %s\n", inputBinFile.relative_path().string().c_str(), outputBinFile.relative_path().string().c_str());
			}
			else
			{
				LOG("Input file %s does not exit\n", inputBinFile.string().c_str());
			}
		}
	}

	void MainMenu::CopyScratchToFinishedProcedure(const SceneInfo* SceneInfo)
	{
		fs::path inputDatFile = fs::path{ "Scratch" } / SceneInfo->GroupKey / SceneInfo->DatArchiveFileName;
		fs::path inputBinFile = fs::path{ "Scratch" } / SceneInfo->GroupKey / SceneInfo->BinArchiveFileName;

		fs::path outputDatFile = fs::path{ "Finished" } / SceneInfo->GroupKey / SceneInfo->DatArchiveFileName;
		fs::path outputBinFile = fs::path{ "Finished" } / SceneInfo->GroupKey / SceneInfo->BinArchiveFileName;

		if (fs::exists(inputDatFile))
		{
			FsUtils::CreateDirIfNotExist(outputDatFile);

			fs::copy(inputDatFile, outputDatFile, fs::copy_options::overwrite_existing);

			LOG("Copied from %s to %s\n", inputDatFile.relative_path().string().c_str(), outputDatFile.relative_path().string().c_str());
		}
		else
		{
			LOG("Input file %s does not exit\n", inputDatFile.string().c_str());
		}

		if (SceneInfo->Type == eSceneTypeLevel)
		{
			if (fs::exists(inputBinFile))
			{
				FsUtils::CreateDirIfNotExist(outputBinFile);

				fs::copy(inputBinFile, outputBinFile, fs::copy_options::overwrite_existing);

				LOG("Copied from %s to %s\n", inputBinFile.relative_path().string().c_str(), outputBinFile.relative_path().string().c_str());
			}
			else
			{
				LOG("Input file %s does not exit\n", inputBinFile.string().c_str());
			}
		}
	}

	void MainMenu::CompareScratchWithFinishedProcedure(const SceneInfo* SceneInfo)
	{
		fs::path leftInputDatFile = fs::path{ "Scratch" } / SceneInfo->GroupKey / SceneInfo->DatArchiveFileName;
		fs::path leftInputBinFile = fs::path{ "Scratch" } / SceneInfo->GroupKey / SceneInfo->BinArchiveFileName;

		fs::path rightInputDatFile = fs::path{ "Finished" } / SceneInfo->GroupKey / SceneInfo->DatArchiveFileName;
		fs::path rightInputBinFile = fs::path{ "Finished" } / SceneInfo->GroupKey / SceneInfo->BinArchiveFileName;

		if (fs::exists(leftInputDatFile) && fs::exists(rightInputDatFile))
		{
			std::vector<U8> leftDatBytes = FsUtils::ReadBinary(leftInputDatFile);
			std::vector<U8> rightDatBytes = FsUtils::ReadBinary(rightInputDatFile);

			std::vector<std::pair<U64, U64>> indices = {};

			if (DiffUtils::Compare(leftDatBytes, rightDatBytes, indices))
			{
				LOG("File missmatch %s\n", leftInputDatFile.relative_path().string().c_str());

				DiffUtils::HexDump(leftDatBytes, rightDatBytes, indices, 1, 128, 16, 6);
			}
			else
			{
				LOG("File matches %s\n", leftInputDatFile.relative_path().string().c_str());
			}
		}

		if (SceneInfo->Type == eSceneTypeLevel)
		{
			return; // TODO

			if (fs::exists(leftInputBinFile) && fs::exists(rightInputBinFile))
			{
				std::vector<U8> leftBinBytes = FsUtils::ReadBinary(leftInputBinFile);
				std::vector<U8> rightBinBytes = FsUtils::ReadBinary(rightInputBinFile);

				std::vector<std::pair<U64, U64>> indices = {};

				if (DiffUtils::Compare(leftBinBytes, rightBinBytes, indices))
				{
					LOG("File missmatch %s\n", leftInputBinFile.relative_path().string().c_str());

					DiffUtils::HexDump(leftBinBytes, rightBinBytes, indices, 1, 128, 16, 6);
				}
				else
				{
					LOG("File matches %s\n", leftInputBinFile.relative_path().string().c_str());
				}
			}
		}
	}

	void MainMenu::PrintTableOfContentProcedure(const SceneInfo* SceneInfo)
	{
		fs::path inputDatFile = gDataDir / SceneInfo->GroupKey / SceneInfo->DatArchiveFileName;
		fs::path inputBinFile = gDataDir / SceneInfo->GroupKey / SceneInfo->BinArchiveFileName;

		std::vector<U8> datBytes = FsUtils::ReadBinary(inputDatFile);

		gBlowFish->Decrypt(datBytes);

		Archive datArchive = {};

		datArchive.DeSerialize(datBytes);
		datArchive.PrintTableOfContent();

		if (fs::exists(inputBinFile))
		{
			std::vector<U8> binBytes = FsUtils::ReadBinary(inputBinFile);

			gBlowFish->Decrypt(binBytes);

			Archive binArchive = {};

			binArchive.DeSerialize(binBytes);
			binArchive.PrintTableOfContent();
		}
	}

	void MainMenu::ExtractToDiskProcedure(const SceneInfo* SceneInfo)
	{
		fs::path inputDatFile = gDataDir / SceneInfo->GroupKey / SceneInfo->DatArchiveFileName;
		fs::path inputBinFile = gDataDir / SceneInfo->GroupKey / SceneInfo->BinArchiveFileName;

		fs::path outputDatFile = fs::path{ "Extracted" } / SceneInfo->DatArchiveFileName;
		fs::path outputBinFile = fs::path{ "Extracted" } / SceneInfo->BinArchiveFileName;

		FsUtils::CreateDirIfNotExist(outputDatFile, true);

		std::vector<U8> datBytes = FsUtils::ReadBinary(inputDatFile);

		gBlowFish->Decrypt(datBytes);

		Archive datArchive = {};

		datArchive.DeSerialize(datBytes);
		datArchive.ExtractToDisk(outputDatFile);

		if (fs::exists(inputBinFile))
		{
			FsUtils::CreateDirIfNotExist(outputBinFile, true);

			std::vector<U8> binBytes = FsUtils::ReadBinary(inputBinFile);

			gBlowFish->Decrypt(binBytes);

			Archive binArchive = {};

			binArchive.DeSerialize(binBytes);
			binArchive.ExtractToDisk(outputBinFile);
		}
	}

	void MainMenu::CreateThumbnailProcedure(const SceneInfo* SceneInfo)
	{
		fs::path thumbnailDir = "Thumbnails";

		FsUtils::CreateDirIfNotExist(thumbnailDir);

		Thumbnail::Generate(*SceneInfo, thumbnailDir);

		LOG("Generating Thumbnail %s\n", SceneInfo->ThumbnailFileName.c_str());
	}
}

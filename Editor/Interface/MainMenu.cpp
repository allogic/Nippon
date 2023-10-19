#include <Common/Macros.h>
#include <Common/Archive.h>
#include <Common/BlowFish.h>

#include <Common/Utilities/FsUtils.h>

#include <Common/Generated/SceneInfos.h>

#include <Editor/Editor.h>
#include <Editor/Scene.h>
#include <Editor/SceneManager.h>
#include <Editor/Thumbnail.h>
#include <Editor/Texture.h>
#include <Editor/TextureLoader.h>

#include <Editor/Interface/MainMenu.h>

#include <Editor/ImGui/imgui.h>

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
				if (ImGui::BeginMenu("Levels"))
				{
					for (const auto& groupInfo : SceneInfos::GetLevelGroups())
					{
						ImGui::PushID(&groupInfo);

						if (ImGui::BeginMenu(groupInfo.MenuName.c_str()))
						{
							for (const auto& sceneInfo : SceneInfos::GetLevelsByGroup(groupInfo))
							{
								ImGui::PushID(&sceneInfo);

								if (ImGui::Selectable(sceneInfo.MenuName.c_str()))
								{
									if (Scene* scene = SceneManager::CreateScene(sceneInfo))
									{
										scene->SetEnableConsole(true);
										scene->SetEnableDebug(true);

										scene->CreateViewport();
										scene->Load();
									}
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
					for (const auto& groupInfo : SceneInfos::GetEntityGroups())
					{
						ImGui::PushID(&groupInfo);

						if (ImGui::BeginMenu(groupInfo.MenuName.c_str()))
						{
							for (const auto& sceneInfo : SceneInfos::GetEntitiesByGroup(groupInfo))
							{
								ImGui::PushID(&sceneInfo);

								if (ImGui::Selectable(sceneInfo.MenuName.c_str()))
								{
									if (Scene* scene = SceneManager::CreateScene(sceneInfo))
									{
										scene->SetEnableConsole(true);
										scene->SetEnableDebug(true);

										scene->CreateViewport();
										scene->Load();
									}
								}

								ImGui::PopID();
							}

							ImGui::EndMenu();
						}

						ImGui::PopID();
					}

					ImGui::EndMenu();
				}

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
				if (ImGui::BeginMenu("Levels"))
				{
					for (const auto& groupInfo : SceneInfos::GetLevelGroups())
					{
						ImGui::PushID(&groupInfo);

						if (ImGui::BeginMenu(groupInfo.MenuName.c_str()))
						{
							for (const auto& sceneInfo : SceneInfos::GetLevelsByGroup(groupInfo))
							{
								ImGui::PushID(&sceneInfo);

								if (ImGui::Selectable(sceneInfo.MenuName.c_str()))
								{
									fs::path relativeDatFileDir = fs::path{ sceneInfo.GroupKey } / sceneInfo.DatArchiveFileName;
									fs::path relativeBinFileDir = fs::path{ sceneInfo.GroupKey } / sceneInfo.BinArchiveFileName;

									fs::path absoluteDatFileDir = gDataDir / relativeDatFileDir;
									fs::path absoluteBinFileDir = gDataDir / relativeBinFileDir;

									BlowFish cipher = gBlowFishKey;

									std::vector<U8> datBytes = FsUtils::ReadBinary(absoluteDatFileDir);

									cipher.Decrypt(datBytes);

									Archive datArchive = datBytes;

									datArchive.Load();

									LOG("\n");
									LOG(" Table Of Content For Level \\%s\\%s .DAT\n", sceneInfo.GroupKey.c_str(), sceneInfo.SceneKey.c_str());
									LOG("=============================================================\n");

									datArchive.DumpTableOfContent(0, 4, 4);

									LOG("\n");

									if (fs::exists(absoluteBinFileDir))
									{
										std::vector<U8> binBytes = FsUtils::ReadBinary(absoluteBinFileDir);

										cipher.Decrypt(binBytes);

										Archive binArchive = binBytes;

										binArchive.Load();

										LOG("\n");
										LOG(" Table Of Content For Level \\%s\\%s .BIN\n", sceneInfo.GroupKey.c_str(), sceneInfo.SceneKey.c_str());
										LOG("=============================================================\n");

										binArchive.DumpTableOfContent(0, 4, 4);

										LOG("\n");
									}
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
					for (const auto& groupInfo : SceneInfos::GetEntityGroups())
					{
						ImGui::PushID(&groupInfo);

						if (ImGui::BeginMenu(groupInfo.MenuName.c_str()))
						{
							for (const auto& sceneInfo : SceneInfos::GetEntitiesByGroup(groupInfo))
							{
								ImGui::PushID(&sceneInfo);

								if (ImGui::Selectable(sceneInfo.MenuName.c_str()))
								{
									fs::path relativeDatFileDir = fs::path{ sceneInfo.GroupKey } / sceneInfo.DatArchiveFileName;

									fs::path absoluteDatFileDir = gDataDir / relativeDatFileDir;

									BlowFish cipher = gBlowFishKey;

									std::vector<U8> datBytes = FsUtils::ReadBinary(absoluteDatFileDir);

									cipher.Decrypt(datBytes);

									Archive datArchive = datBytes;

									datArchive.Load();

									LOG("\n");
									LOG(" Table Of Content For Entity \\%s\\%s .DAT\n", sceneInfo.GroupKey.c_str(), sceneInfo.SceneKey.c_str());
									LOG("=============================================================\n");

									datArchive.DumpTableOfContent(0, 4, 4);

									LOG("\n");
								}

								ImGui::PopID();
							}

							ImGui::EndMenu();
						}

						ImGui::PopID();
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Dump To Disk"))
			{
				if (ImGui::BeginMenu("Levels"))
				{
					for (const auto& groupInfo : SceneInfos::GetLevelGroups())
					{
						ImGui::PushID(&groupInfo);

						if (ImGui::BeginMenu(groupInfo.MenuName.c_str()))
						{
							for (const auto& sceneInfo : SceneInfos::GetLevelsByGroup(groupInfo))
							{
								ImGui::PushID(&sceneInfo);

								if (ImGui::Selectable(sceneInfo.MenuName.c_str()))
								{
									fs::path exportDatDir = fs::path{ "Dumps" } / "Levels" / sceneInfo.DatArchiveFileName;
									fs::path exportBinDir = fs::path{ "Dumps" } / "Levels" / sceneInfo.BinArchiveFileName;

									fs::path relativeDatFileDir = fs::path{ sceneInfo.GroupKey } / sceneInfo.DatArchiveFileName;
									fs::path relativeBinFileDir = fs::path{ sceneInfo.GroupKey } / sceneInfo.BinArchiveFileName;

									fs::path absoluteDatFileDir = gDataDir / relativeDatFileDir;
									fs::path absoluteBinFileDir = gDataDir / relativeBinFileDir;

									BlowFish cipher = gBlowFishKey;

									FsUtils::CreateIfNotExist(exportDatDir, true);

									std::vector<U8> datBytes = FsUtils::ReadBinary(absoluteDatFileDir);

									cipher.Decrypt(datBytes);

									Archive datArchive = datBytes;

									datArchive.Load();
									datArchive.DumpToDiskRecursive(exportDatDir);

									if (fs::exists(absoluteBinFileDir))
									{
										FsUtils::CreateIfNotExist(exportBinDir, true);

										std::vector<U8> binBytes = FsUtils::ReadBinary(absoluteBinFileDir);

										cipher.Decrypt(binBytes);

										Archive binArchive = binBytes;

										binArchive.Load();
										binArchive.DumpToDiskRecursive(exportBinDir);
									}
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
					for (const auto& groupInfo : SceneInfos::GetEntityGroups())
					{
						ImGui::PushID(&groupInfo);

						if (ImGui::BeginMenu(groupInfo.MenuName.c_str()))
						{
							for (const auto& sceneInfo : SceneInfos::GetEntitiesByGroup(groupInfo))
							{
								ImGui::PushID(&sceneInfo);

								if (ImGui::Selectable(sceneInfo.MenuName.c_str()))
								{
									fs::path exportDatDir = fs::path{ "Dumps" } / "Entities" / sceneInfo.DatArchiveFileName;

									fs::path relativeDatFileDir = fs::path{ sceneInfo.GroupKey } / sceneInfo.DatArchiveFileName;

									fs::path absoluteDatFileDir = gDataDir / relativeDatFileDir;

									BlowFish cipher = gBlowFishKey;

									FsUtils::CreateIfNotExist(exportDatDir, true);

									std::vector<U8> datBytes = FsUtils::ReadBinary(absoluteDatFileDir);

									cipher.Decrypt(datBytes);

									Archive datArchive = datBytes;

									datArchive.Load();
									datArchive.DumpToDiskRecursive(exportDatDir);
								}

								ImGui::PopID();
							}

							ImGui::EndMenu();
						}

						ImGui::PopID();
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}
	}

	void MainMenu::RenderToolsMenu()
	{
		if (ImGui::BeginMenu("Tools"))
		{
			if (ImGui::Selectable("Generate Thumbnails"))
			{
				for (const auto& groupInfo : SceneInfos::GetLevelGroups())
				{
					LOG("\n");
					LOG(" Generating Thumbnails For Level Group \\%s\\*\n", groupInfo.GroupKey.c_str());
					LOG("=============================================================\n");

					for (const auto& sceneInfo : SceneInfos::GetLevelsByGroup(groupInfo))
					{
						Thumbnail::Generate(sceneInfo);

						LOG("    Generating %s\n", sceneInfo.ThumbnailFileName.c_str());
					}

					LOG("\n");
				}

				for (const auto& groupInfo : SceneInfos::GetEntityGroups())
				{
					LOG("\n");
					LOG(" Generating Thumbnails For Entity Group \\%s\\*\n", groupInfo.GroupKey.c_str());
					LOG("=============================================================\n");

					for (const auto& sceneInfo : SceneInfos::GetEntitiesByGroup(groupInfo))
					{
						Thumbnail::Generate(sceneInfo);

						LOG("    Generating %s\n", sceneInfo.ThumbnailFileName.c_str());
					}

					LOG("\n");
				}
			}

			ImGui::EndMenu();
		}
	}
}

#include <Common/Macros.h>

#include <Editor/Editor.h>
#include <Editor/Scene.h>
#include <Editor/SceneManager.h>
#include <Editor/Thumbnail.h>
#include <Editor/Texture.h>
#include <Editor/TextureLoader.h>

#include <Editor/Interface/MainMenu.h>

#include <Editor/Generated/SceneInfos.h>

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
		RenderLevelMenu();
		RenderEntityMenu();
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

	void MainMenu::RenderLevelMenu()
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
	}

	void MainMenu::RenderEntityMenu()
	{
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
						LOG("    Generated %s\n", sceneInfo.ThumbnailFileName.c_str());

						Thumbnail::Generate(sceneInfo);
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
						LOG("    Generated %s\n", sceneInfo.ThumbnailFileName.c_str());

						Thumbnail::Generate(sceneInfo);
					}

					LOG("\n");
				}

				LOG("Finished\n");
				LOG("\n");
			}

			ImGui::EndMenu();
		}
	}
}

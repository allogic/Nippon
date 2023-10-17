#include <Common/Utilities/FsUtils.h>
#include <Common/Utilities/TextureUtils.h>

#include <Editor/Editor.h>
#include <Editor/Scene.h>
#include <Editor/SceneManager.h>
#include <Editor/InterfaceManager.h>
#include <Editor/Texture.h>

#include <Editor/Exporter/WavefrontExporter.h>

#include <Editor/Interface/LevelBrowser.h>

#include <Editor/Generated/SceneInfos.h>

#include <Editor/ImGui/imgui.h>

namespace ark
{
	LevelBrowser::LevelBrowser()
	{

	}

	LevelBrowser::~LevelBrowser()
	{

	}

	void LevelBrowser::Reset()
	{

	}

	void LevelBrowser::Render()
	{
		ImGui::Begin("Level Browser");

		if (ImGui::BeginTabBar("Levels"))
		{
			for (const auto& groupInfo : SceneInfos::GetLevelGroups())
			{
				ImGui::PushID(&groupInfo);

				if (ImGui::BeginTabItem(groupInfo.GroupKey.c_str()))
				{
					ImGui::BeginChild("Level Group");

					const auto& sceneInfos = SceneInfos::GetLevelsByGroup(groupInfo);

					for (U32 i = 0; i < sceneInfos.size(); i++)
					{
						ImGui::PushID(&sceneInfos[i]);

						if ((i > 0) && ((i % 3) != 0))
						{
							ImGui::SameLine(0, 0);
						}

						ImTextureID textureId = nullptr;

						if (Texture2D* texture = InterfaceManager::GetLevelThumbnail(sceneInfos[i].ThumbnailFileName))
						{
							textureId = (ImTextureID)(U64)texture->GetId();
						}

						if (ImGui::ImageButton(textureId, ImVec2{ 128.0F, 128.0F }))
						{
							if (Scene* scene = SceneManager::CreateScene(sceneInfos[i]))
							{
								scene->SetEnableConsole(true);
								scene->SetEnableDebug(true);

								scene->CreateViewport();
								scene->Load();
							}
						}

						ImGui::PopID();
					}

					ImGui::EndChild();
					ImGui::EndTabItem();
				}

				ImGui::PopID();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}
}

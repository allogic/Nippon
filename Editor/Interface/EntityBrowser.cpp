#include <Common/Utilities/FsUtils.h>
#include <Common/Utilities/TextureUtils.h>

#include <Common/Generated/SceneInfos.h>

#include <Editor/Editor.h>
#include <Editor/Scene.h>
#include <Editor/SceneManager.h>
#include <Editor/InterfaceManager.h>
#include <Editor/Texture2D.h>

#include <Editor/Exporter/WavefrontExporter.h>

#include <Editor/Interface/EntityBrowser.h>

#include <Editor/ImGui/imgui.h>

namespace ark
{
	EntityBrowser::EntityBrowser()
	{

	}

	EntityBrowser::~EntityBrowser()
	{
		
	}

	void EntityBrowser::Reset()
	{

	}

	void EntityBrowser::Render()
	{
		ImGui::Begin("Entity Browser");

		if (ImGui::BeginTabBar("Entities"))
		{
			for (const auto& groupInfo : SceneInfos::GetEntityGroups())
			{
				ImGui::PushID(&groupInfo);

				if (ImGui::BeginTabItem(groupInfo.GroupKey.c_str()))
				{
					ImGui::BeginChild("Level Group");

					const auto& sceneInfos = SceneInfos::GetEntitiesByGroup(groupInfo);

					for (U32 i = 0; i < sceneInfos.size(); i++)
					{
						ImGui::PushID(&sceneInfos[i]);

						if ((i > 0) && ((i % 3) != 0))
						{
							ImGui::SameLine(0, 0);
						}

						ImTextureID textureId = nullptr;

						if (U32 texture = InterfaceManager::GetEntityThumbnail(sceneInfos[i].ThumbnailFileName))
						{
							textureId = (ImTextureID)(U64)texture;
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

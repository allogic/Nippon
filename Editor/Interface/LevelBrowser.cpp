#include <Common/Utilities/FsUtils.h>

#include <Editor/Editor.h>
#include <Editor/Scene.h>
#include <Editor/SceneManager.h>
#include <Editor/InterfaceManager.h>
#include <Editor/Texture2D.h>

#include <Editor/Databases/FileDatabase.h>
#include <Editor/Databases/ThumbnailDatabase.h>

#include <Editor/Exporter/WavefrontExporter.h>

#include <Editor/Interface/LevelBrowser.h>

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
			for (const auto& directory : FileDatabase::GetLevelDirectories())
			{
				ImGui::PushID(directory);

				if (ImGui::BeginTabItem(FileDatabase::GetDirectoryIdByDirectory(directory)))
				{
					ImGui::BeginChild("Level Directory");

					const auto& fileContainers = FileDatabase::GetLevelFileContainersByDirectory(directory);

					for (U32 i = 0; i < fileContainers.size(); i++)
					{
						ImGui::PushID(&fileContainers[i]);

						if ((i > 0) && ((i % 3) != 0))
						{
							ImGui::SameLine(0, 0);
						}

						ImTextureID textureId = nullptr;

						const auto& thumbnailContainer = ThumbnailDatabase::GetThumbnailContainerByIdentifier(fileContainers[i]->GetIdentifier());

						if (U32 texture = thumbnailContainer->GetTexture())
						{
							textureId = (ImTextureID)(U64)texture;
						}

						if (ImGui::ImageButton(textureId, ImVec2{ 128.0F, 128.0F }))
						{
							if (Scene* scene = SceneManager::CreateScene(fileContainers[i]))
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

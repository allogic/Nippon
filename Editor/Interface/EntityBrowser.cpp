#include <Common/Utilities/FsUtils.h>

#include <Editor/Editor.h>
#include <Editor/Scene.h>
#include <Editor/SceneManager.h>
#include <Editor/InterfaceManager.h>
#include <Editor/Texture2D.h>

#include <Editor/Databases/FileDatabase.h>
#include <Editor/Databases/ThumbnailDatabase.h>

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
			for (const auto& directory : FileDatabase::GetEntityDirectories())
			{
				ImGui::PushID(directory);

				if (ImGui::BeginTabItem(FileDatabase::GetDirectoryIdByDirectory(directory)))
				{
					ImGui::BeginChild("Entity Directory");

					const auto& fileContainers = FileDatabase::GetEntityFileContainersByDirectory(directory);

					for (U32 i = 0; i < fileContainers.size(); i++)
					{
						ImGui::PushID(&fileContainers[i]);

						if ((i > 0) && ((i % 3) != 0))
						{
							ImGui::SameLine(0, 0);
						}

						ImTextureID textureId = nullptr;

						if (const ThumbnailContainer* thumbnailContainer = ThumbnailDatabase::GetThumbnailContainerByIdentifier(fileContainers[i]->GetIdentifier()))
						{
							if (U32 texture = thumbnailContainer->GetTexture())
							{
								textureId = (ImTextureID)(U64)texture;
							}
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

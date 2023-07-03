#include <Common/Recursion/FileTreeNode.h>

#include <Common/Utils/FsUtils.h>
#include <Common/Utils/TextureUtils.h>

#include <Editor/Editor.h>
#include <Editor/SceneManager.h>
#include <Editor/Texture.h>

#include <Editor/Exporter/WavefrontExporter.h>

#include <Editor/Interface/LevelBrowser.h>

#include <Vendor/ImGui/imgui.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	LevelBrowser::LevelBrowser()
	{

	}

	LevelBrowser::~LevelBrowser()
	{
		for (auto& [entry, entries] : mThumbnails)
		{
			for (auto& [subEntry, texture] : entries)
			{
				delete texture;
				texture = nullptr;
			}
		}
	}

	void LevelBrowser::Reset()
	{

	}

	void LevelBrowser::Draw()
	{
		ImGui::Begin("Level Browser");

		if (ImGui::BeginTabBar("Levels"))
		{
			for (auto entryIt = gArchive["regions"].MemberBegin(); entryIt != gArchive["regions"].MemberEnd(); entryIt++)
			{
				ImGui::PushID(entryIt->name.GetString());

				std::string entryDir = entryIt->name.GetString();

				if (ImGui::BeginTabItem(entryDir.c_str()))
				{
					if (ImGui::BeginTable("#Levels", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersInnerV))
					{
						ImGui::TableSetupColumn("Thumbnail", ImGuiTableColumnFlags_WidthFixed, 195.0F);
						ImGui::TableSetupColumn("Metadata", ImGuiTableColumnFlags_WidthStretch);
						ImGui::TableSetupScrollFreeze(0, 1);
						ImGui::TableHeadersRow();

						rj::Value& subEntries = entryIt->value["entries"];

						for (auto subEntryIt = subEntries.MemberBegin(); subEntryIt != subEntries.MemberEnd(); subEntryIt++)
						{
							ImGui::PushID(subEntryIt->name.GetString());
							ImGui::TableNextRow();
							ImGui::TableNextColumn();

							std::string subEntryDir = subEntryIt->name.GetString();
							std::string subEntryName = subEntryIt->value["name"].GetString();
							std::string subEntryInfo = subEntryIt->value["info"].GetString();

							fs::path thumbnail = fs::path{ gConfig["thumbnailDir"].GetString() } / entryDir / subEntryDir / fs::path{ "thumb.png" };

							auto& texture = mThumbnails[entryDir][subEntryDir];

							if (fs::exists(thumbnail))
							{
								if (!texture)
								{
									texture = TextureUtils::LoadGeneric(thumbnail);
								}
							}

							if (texture)
							{
								ImGui::Image((void*)(U64)texture->GetId(), ImVec2{ 192.0F, 192.0F });
							}
							else
							{
								ImGui::Image(nullptr, ImVec2{ 192.0F, 192.0F });
							}

							ImGui::TableNextColumn();

							ImGui::Text(subEntryDir.c_str());
							ImGui::TextWrapped(subEntryName.c_str());
							ImGui::TextWrapped(subEntryInfo.c_str());

							if (ImGui::Button("Open"))
							{
								SceneManager::CreateLevel(entryDir, subEntryDir, subEntryIt->value);
							}

							if (ImGui::Button("Export"))
							{
								WavefrontExporter::ExportLevel(entryDir, subEntryDir);
							}

							ImGui::PopID();
						}

						ImGui::EndTable();
					}

					ImGui::EndTabItem();
				}

				ImGui::PopID();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}
}
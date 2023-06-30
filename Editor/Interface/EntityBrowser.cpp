#include <Common/Recursion/FileTreeNode.h>

#include <Common/Utils/TextureUtils.h>

#include <Editor/Texture.h>

#include <Editor/Interface/EntityBrowser.h>

#include <Vendor/ImGui/imgui.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern rj::Document gArchive;
extern rj::Document gConfig;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  EntityBrowser::EntityBrowser()
  {

  }

  EntityBrowser::~EntityBrowser()
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

  void EntityBrowser::Reset()
  {

  }

  void EntityBrowser::Draw()
  {
    ImGui::Begin("Entity Browser");

    if (ImGui::BeginTabBar("Entries"))
    {
      for (auto entryIt = gArchive["entities"].MemberBegin(); entryIt != gArchive["entities"].MemberEnd(); entryIt++)
      {
        ImGui::PushID(&entryIt);

        std::string entryDir = entryIt->name.GetString();

        if (ImGui::BeginTabItem(entryDir.c_str()))
        {
          if (ImGui::BeginTable("#Entities", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersInnerV))
          {
            ImGui::TableSetupColumn("Thumbnail", ImGuiTableColumnFlags_WidthFixed, 195.0F);
            ImGui::TableSetupColumn("Metadata", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableHeadersRow();

            rj::Value& subEntries = entryIt->value["entries"];

            for (auto subEntryIt = subEntries.MemberBegin(); subEntryIt != subEntries.MemberEnd(); subEntryIt++)
            {
              ImGui::TableNextRow();
              ImGui::TableNextColumn();

              std::string subEntryDir = subEntryIt->name.GetString();
              std::string subEntryName = subEntryIt->value["name"].GetString();

              fs::path thumbnail = fs::path{ gConfig["unpackDir"].GetString() } / entryDir / subEntryDir / fs::path{ "thumb.png" };

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
              ImGui::Text(subEntryName.c_str());

              ImGui::Separator();

              if (ImGui::Button("Export as OBJ"))
              {

              }
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
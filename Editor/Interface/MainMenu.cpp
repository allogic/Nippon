#include <Common/Debug.h>

#include <Editor/Scene.h>
#include <Editor/Packer.h>

#include <Editor/Interface/MainMenu.h>

#include <Vendor/ImGui/imgui.h>

#include <Vendor/rapidjson/document.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern rj::Document gTranslation;

extern ark::Scene* gScene;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  void MainMenu::Draw()
  {
    ImGui::BeginMainMenuBar();

    for (const auto& translation : gTranslation.GetArray())
    {
      if (ImGui::BeginMenu(translation["name"].GetString()))
      {
        std::string entryType = translation["type"].GetString();

        for (auto entryIt = translation["entries"].MemberBegin(); entryIt != translation["entries"].MemberEnd(); entryIt++)
        {
          std::string entryDir = entryIt->name.GetString();
          std::string entryName = entryIt->value["name"].GetString();

          if (entryName == "") entryName = entryDir;

          if (ImGui::BeginMenu(entryName.c_str()))
          {
            if (ImGui::Selectable("For Each"))
            {
              for (auto subEntryIt = entryIt->value["entries"].MemberBegin(); subEntryIt != entryIt->value["entries"].MemberEnd(); subEntryIt++)
              {
                std::string subEntryDir = subEntryIt->name.GetString();

                SwitchScene(entryDir, subEntryDir, entryType);
              }
            }

            ImGui::Separator();

            for (auto subEntryIt = entryIt->value["entries"].MemberBegin(); subEntryIt != entryIt->value["entries"].MemberEnd(); subEntryIt++)
            {
              std::string subEntryDir = subEntryIt->name.GetString();
              std::string subEntryName = subEntryIt->value["name"].GetString();

              if (subEntryName == "") subEntryName = subEntryDir;

              ImGui::PushID(subEntryDir.c_str());
              if (ImGui::Selectable(subEntryName.c_str()))
              {
                SwitchScene(entryDir, subEntryDir, entryType);
              }

              ImGui::PopID();
            }

            ImGui::EndMenu();
          }
        }        

        ImGui::EndMenu();
      }
    }

    if (ImGui::BeginMenu("Packer"))
    {
      if (ImGui::Selectable("Unpack"))
      {
        Packer::Unpack();
      }

      if (ImGui::Selectable("Repack"))
      {
        Packer::Repack();
      }

      ImGui::Separator();

      if (ImGui::Selectable("Check Integrity"))
      {
        Packer::CheckIntegrity();
      }

      if (ImGui::Selectable("Generate Integrity"))
      {
        Packer::GenerateIntegrityMap();
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  void MainMenu::SwitchScene(const std::string& EntryDir, const std::string& SubEntryDir, const std::string& EntryType)
  {
    U32 prevWidth = 1;
    U32 prevHeight = 1;

    if (gScene)
    {
      prevWidth = gScene->GetWidth();
      prevHeight = gScene->GetHeight();

      delete gScene;
      gScene = nullptr;
    }

    gScene = new Scene{ EntryDir, SubEntryDir, EntryType };

    if (gScene)
    {
      gScene->Resize(prevWidth, prevHeight);
    }
  }
}
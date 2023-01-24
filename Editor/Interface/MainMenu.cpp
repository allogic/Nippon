#include <Editor/Scene.h>
#include <Editor/Packer.h>

#include <Editor/Interface/MainMenu.h>

#include <Vendor/ImGui/imgui.h>

#include <Vendor/rapidjson/document.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern rj::Document gWorld;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  void MainMenu::Update()
  {

  }

  void MainMenu::Draw()
  {
    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("File"))
    {
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Level"))
    {
      const rj::Value& regions = gWorld["regions"];

      for (auto regionIt = regions.MemberBegin(); regionIt != regions.MemberEnd(); regionIt++)
      {
        std::string regionId = regionIt->name.GetString();
        std::string regionName = regionIt->value["name"].GetString();

        if (ImGui::BeginMenu(regionName.c_str()))
        {
          const rj::Value& levels = regionIt->value["levels"];

          for (auto levelIt = levels.MemberBegin(); levelIt != levels.MemberEnd(); levelIt++)
          {
            std::string levelId = levelIt->name.GetString();
            std::string levelName = levelIt->value["name"].GetString();

            if (ImGui::Selectable(levelName.c_str(), false))
            {
              Scene::Destroy();
              Scene::Create(regionId, levelId);
            }
          }

          ImGui::EndMenu();
        }
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Packer"))
    {
      if (ImGui::Selectable("Unpack", false))
      {
        Packer::Unpack();
      }

      if (ImGui::Selectable("Repack", false))
      {
        Packer::Repack();
      }

      ImGui::Separator();

      if (ImGui::Selectable("Check Integrity", false))
      {
        Packer::CheckIntegrity();
      }

      if (ImGui::Selectable("Generate Integrity", false))
      {
        Packer::GenerateIntegrityMap();
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}
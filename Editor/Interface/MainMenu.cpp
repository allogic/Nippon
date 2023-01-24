#include <string>
#include <map>

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
      if (ImGui::Selectable("Generate Integrity Map", false))
      {
        Packer::GenerateIntegrityMap();
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Level"))
    {
      for (const auto& region : gWorld["regions"].GetArray())
      {
        if (ImGui::BeginMenu(region["name"].GetString()))
        {
          for (const auto& level : region["levels"].GetArray())
          {
            if (ImGui::Selectable(level["name"].GetString(), false))
            {
              Scene::Destroy();
              Scene::Create(region["id"].GetString(), level["id"].GetString());
            }
          }

          ImGui::EndMenu();
        }
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}
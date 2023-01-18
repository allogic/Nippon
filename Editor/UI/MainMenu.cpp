#include <string>
#include <map>

#include <Editor/Scene.h>

#include <Editor/UI/MainMenu.h>

#include <Vendor/ImGui/imgui.h>

#include <Vendor/rapidjson/document.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern rapidjson::Document gWorld;

extern ark::Scene* gScene;

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

    if (ImGui::BeginMenu("Levels"))
    {
      for (const auto& region : gWorld["regions"].GetArray())
      {
        if (ImGui::BeginMenu(region["name"].GetString()))
        {
          for (const auto& level : region["levels"].GetArray())
          {
            if (ImGui::Selectable(level["name"].GetString(), false))
            {
              if (gScene)
              {
                delete gScene;
              }

              gScene = new Scene{ region["id"].GetString(), level["id"].GetString() };
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
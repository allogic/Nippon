#include <string>
#include <map>

#include <Editor/Scene.h>

#include <Editor/UI/MainMenu.h>

#include <Vendor/ImGui/imgui.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern ark::Scene* gScene;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  static std::map<std::string, std::string> sRegions =
  {
    { "0", "Developer Maps" },
    { "1", "Eastern Nippon" },
    { "2", "Western Nippon" },
    { "3", "Kamui" },
    { "c", "Title Screens" },
    { "d", "Beta Maps" },
    { "e", "Fishing Maps" },
    { "f", "Overworld Maps" },
  };

  static std::map<std::string, std::map<std::string, std::string>> sLevels =
  {
    {
      "0",
      {
        { "06", "Endless load screen" },
        { "07", "2005 Prototype Intro Cutscene Waka Lake" },
        { "10", "Item Test Map - Items 0x00 - 0x7F" },
        { "11", "Item Test Map - Items 0x80 - 0xFF" },
        { "20", "Brush Interaction Test Map 1 - Large Animals" },
        { "21", "Brush Interaction Test Map 2 - Small Animals" },
        { "30", "Brush Interaction Test Map 3 - Wheel Enemies" },
        { "31", "Brush Interaction Test Map 4 - Bull Enemies" },
        { "32", "Brush Interaction Test Map 5 - Empty" },
        { "33", "Brush Interaction Test Map 6 - Empty" },
        { "34", "Brush Interaction Test Map 7 - Blue Cylinder" },
        { "ff", "Collision and Movement Test Map" },
      }
    },
    {
      "1",
      {
        { "00", "Kamiki Village - Game Start" },
        { "01", "Cave of Nagi" },
        { "02", "Kamiki Village" },
        { "03", "Hana Valley" },
        { "04", "Tsuta Ruins" },
        { "05", "City Checkpoint" },
        { "06", "Tsuta Ruins - Spider Queen Arena" },
        { "07", "Gale Shrine" },
        { "08", "Kusa Village" },
        { "09", "Sasa Sanctuary" },
        { "0a", "Agata Forest - Madame Fawn's House" },
        { "0b", "Digging Minigame" },
        { "0c", "Onigiri Dojo Lesson Room" },
        { "0d", "Gale Shrine - Crimson Helm Arena" },
        { "0e", "Calcified Cavern" },
        { "10", "Moon Cave - Interior" },
        { "11", "Moon Cave - Staircase and Orochi Arena" },
        { "12", "Kamiki Village - After fighting Tei" },
        { "13", "Ryoshima Coast - Bandit Spider Arena" },
        { "14", "Taka Pass - Stray Bead #26 Cave" },
        { "15", "Kamui - Cherry Bomb 3 Cave" },
        { "16", "N.Ryoshima Coast - Power Slash 2 Cave" },
        { "17", "N.Ryoshima Coast - Cherry Bomb 2 Cave" },
        { "18", "Kamui(Ezofuji) - Power Slash 3 Cave" },
        { "19", "Kamui - Blockhead Grande Cave" },
        { "1a", "N.Ryoshima Coast - Bandit Spider Arena" },
        { "1b", "Kamui - Bandit Spider Arena" },
        { "1c", "Dragon Palace - Stray Bead #70 Cave" },
        { "1d", "N.Ryoshima Coast - Stray Bead #63 Cave" },
        { "20", "New Game Intro Cutscene Map" },
        { "21", "Beta Kamiki" },
        { "22", "River of the Heavens" },
      }
    },
    {
      "2",
      {
        { "00", "Sei - an City(Aristocratic Qtr.)" },
        { "01", "Sei - an City(Commoners' Qtr.)" },
        { "02", "Sei - an City(Aristocratic Qtr.) - Himiko's Palace" },
        { "03", "Dragon Palace" },
        { "04", "Inside the Dragon" },
        { "05", "Sunken Ship" },
        { "06", "Imperial Palace - Ammy Size" },
        { "07", "Imperial Palace - Issun Size" },
        { "08", "Oni Island - Lower Interior" },
        { "09", "Oni Island - Ninetales Arena" },
        { "0a", "Catcall Tower" },
        { "0b", "Imperial Palace - Blight Arena" },
        { "0c", "Sei - an City(Commoners' Qtr.) - Kimono Shop" },
        { "0d", "Oni Island - Exterior" },
        { "0e", "Oni Island - Upper Interior" },
        { "0f", "Oni Island - Sidescroller" },
      }
    },
    {
      "3",
      {
        { "01", "Wep'keer" },
        { "02", "Kamiki Village - 100 years ago" },
        { "03", "Wawku Shrine" },
        { "04", "Wawku Shrine - Nechku Arena" },
        { "05", "Ponc'tan" },
        { "06", "Moon Cave - 100 years ago" },
        { "07", "Ark of Yamato" },
        { "08", "Ark of Yamato - Spider Queen Arena" },
        { "09", "Ark of Yamato - Orochi Arena" },
        { "0a", "Ark of Yamato - Blight Arena" },
        { "0b", "Ark of Yamato - Ninetales Arena" },
        { "0c", "Ark of Yamato - Crimson Helm Arena" },
        { "0d", "Ponc'tan - Mrs. Seal's House" },
        { "10", "Yoshpet" },
        { "11", "Inner Yoshpet" },
        { "12", "Ark of Yamato - Yami Arena" },
        { "13", "Wep'keer Square" },
        { "14", "Wawku Shrine - Lechku & Nechku Arena" },
      }
    },
    {
      "c",
      {
        { "00", "Return to Title Screen" },
        { "02", "Presents from Issun" },
      }
    },
    {
      "d",
      {
        { "00", "Beta Shinshu" },
        { "01", "Beta Hana" },
        { "02", "Beta Tsuta" },
        { "03", "Beta Agata" },
        { "04", "Beta Ryoshima" },
        { "05", "Beta Kamui" },
        { "06", "Beta Taka" },
        { "07", "Title Screen Demo Cutscene Village" },
      }
    },
    {
      "e",
      {
        { "00", "Fishing with Benkei on Sei - an Bridge" },
        { "01", "Fishing with Benkei next to Himiko's Palace" },
        { "02", "Fishing with Kokari in Agata" },
        { "03", "Fishing with Kokari in Kamui" },
        { "04", "Fishing with Fisherman in NRyo" },
      }
    },
    {
      "f",
      {
        { "01", "Shinshu Field - Cursed" },
        { "02", "Shinshu Field - Healed" },
        { "03", "Agata Forest - Cursed" },
        { "04", "Agata Forest - Healed" },
        { "06", "Moon Cave Entrance" },
        { "07", "Taka Pass - Cursed" },
        { "08", "Taka Pass - Healed" },
        { "09", "Ryoshima Coast - Cursed" },
        { "0a", "Ryoshima Coast - Healed" },
        { "0c", "N.Ryoshima Coast" },
        { "11", "Kamui - Cursed" },
        { "12", "Kamui - Healed" },
        { "13", "Kamui(Ezofuji)" },
        { "20", "Shinshu Field - 100 years ago" },
        { "21", "Moon Cave Entrance - 100 years ago" },
      }
    },    
  };

  void MainMenu::Update()
  {

  }

  void MainMenu::Draw()
  {
    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("Levels"))
    {
      for (const auto& [regionId, regionName] : sRegions)
      {
        if (ImGui::BeginMenu(regionName.c_str()))
        {
          for (const auto& [levelId, levelName] : sLevels[regionId])
          {
            if (ImGui::Selectable(levelName.c_str(), false))
            {
              if (gScene)
              {
                delete gScene;
              }

              gScene = new Scene{ regionId, levelId };
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
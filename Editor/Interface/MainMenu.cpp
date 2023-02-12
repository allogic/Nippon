#include <Common/Debug.h>

#include <Editor/Scene.h>
#include <Editor/Packer.h>

#include <Editor/Interface/MainMenu.h>

#include <Vendor/ImGui/imgui.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern rj::Document gArchive;

extern ark::Scene* gScene;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  void MainMenu::Draw()
  {
    ImGui::BeginMainMenuBar();

    DrawMapMenu();
    DrawPackerMenu();

    ImGui::EndMainMenuBar();
  }

  void MainMenu::DrawMapMenu()
  {
    if (ImGui::BeginMenu("Scene"))
    {
      DrawMenuTree("Regions", gArchive["regions"], MainMenu::MapItem);

      ImGui::EndMenu();
    }
  }

  void MainMenu::DrawPackerMenu()
  {
    if (ImGui::BeginMenu("Packer"))
    {
      if (ImGui::BeginMenu("Decrypt"))
      {
        if (ImGui::Selectable("All"))
        {
          DoProcFor(gArchive["regions"], MainMenu::DecryptItem);
          DoProcFor(gArchive["characters"], MainMenu::DecryptItem);
          DoProcFor(gArchive["items"], MainMenu::DecryptItem);
          DoProcFor(gArchive["unknown"], MainMenu::DecryptItem);
        }

        DrawMenuTree("Regions", gArchive["regions"], MainMenu::DecryptItem);
        DrawMenuTree("Characters", gArchive["characters"], MainMenu::DecryptItem);
        DrawMenuTree("Items", gArchive["items"], MainMenu::DecryptItem);
        DrawMenuTree("Unknown", gArchive["unknown"], MainMenu::DecryptItem);

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Unpack"))
      {
        if (ImGui::Selectable("All"))
        {
          DoProcFor(gArchive["regions"], MainMenu::UnpackItem);
          DoProcFor(gArchive["characters"], MainMenu::UnpackItem);
          DoProcFor(gArchive["items"], MainMenu::UnpackItem);
          DoProcFor(gArchive["unknown"], MainMenu::UnpackItem);
        }

        DrawMenuTree("Regions", gArchive["regions"], MainMenu::UnpackItem);
        DrawMenuTree("Characters", gArchive["characters"], MainMenu::UnpackItem);
        DrawMenuTree("Items", gArchive["items"], MainMenu::UnpackItem);
        DrawMenuTree("Unknown", gArchive["unknown"], MainMenu::UnpackItem);

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Repack"))
      {
        if (ImGui::Selectable("All"))
        {
          DoProcFor(gArchive["regions"], MainMenu::RepackItem);
          DoProcFor(gArchive["characters"], MainMenu::RepackItem);
          DoProcFor(gArchive["items"], MainMenu::RepackItem);
          DoProcFor(gArchive["unknown"], MainMenu::RepackItem);
        }

        DrawMenuTree("Regions", gArchive["regions"], MainMenu::RepackItem);
        DrawMenuTree("Characters", gArchive["characters"], MainMenu::RepackItem);
        DrawMenuTree("Items", gArchive["items"], MainMenu::RepackItem);
        DrawMenuTree("Unknown", gArchive["unknown"], MainMenu::RepackItem);

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Encrypt"))
      {
        if (ImGui::Selectable("All"))
        {
          DoProcFor(gArchive["regions"], MainMenu::EncryptItem);
          DoProcFor(gArchive["characters"], MainMenu::EncryptItem);
          DoProcFor(gArchive["items"], MainMenu::EncryptItem);
          DoProcFor(gArchive["unknown"], MainMenu::EncryptItem);
        }

        DrawMenuTree("Regions", gArchive["regions"], MainMenu::EncryptItem);
        DrawMenuTree("Characters", gArchive["characters"], MainMenu::EncryptItem);
        DrawMenuTree("Items", gArchive["items"], MainMenu::EncryptItem);
        DrawMenuTree("Unknown", gArchive["unknown"], MainMenu::EncryptItem);

        ImGui::EndMenu();
      }

      ImGui::EndMenu();
    }
  }

  void MainMenu::MapItem(const std::string& Entry, const std::string& SubEntry)
  {
    LOG("Switching scene, please wait...\n");

    Scene::Switch(Entry, SubEntry);

    LOG("Scene switched\n");
    LOG("\n");
  }

  void MainMenu::DecryptItem(const std::string& Entry, const std::string& SubEntry)
  {
    LOG("Decrypting, please wait...\n");

    Packer::DecryptArchive(Entry, SubEntry);

    LOG("Decryption finished\n");
    LOG("\n");
  }

  void MainMenu::EncryptItem(const std::string& Entry, const std::string& SubEntry)
  {
    LOG("Encrypting, please wait...\n");

    Packer::EncryptArchive(Entry, SubEntry);

    LOG("Encryption finished\n");
    LOG("\n");
  }

  void MainMenu::UnpackItem(const std::string& Entry, const std::string& SubEntry)
  {
    LOG("Unpacking, please wait...\n");

    Packer::UnpackArchive(Entry, SubEntry);

    LOG("Unpacking finished\n");
    LOG("\n");
  }

  void MainMenu::RepackItem(const std::string& Entry, const std::string& SubEntry)
  {
    LOG("Repacking, please wait...\n");

    Packer::RepackArchive(Entry, SubEntry);

    LOG("Repacking finished\n");
    LOG("\n");
  }

  void MainMenu::DrawMenuTree(const std::string& Name, rj::Value& Entry, MenuItemProc Procedure)
  {
    ImGui::PushID(&Entry);

    if (ImGui::BeginMenu(Name.c_str()))
    {
      if (ImGui::Selectable("All"))
      {
        for (auto entryIt = Entry.MemberBegin(); entryIt != Entry.MemberEnd(); entryIt++)
        {
          std::string entryDir = entryIt->name.GetString();

          rj::Value& subEntries = entryIt->value["entries"];

          for (auto subEntryIt = subEntries.MemberBegin(); subEntryIt != subEntries.MemberEnd(); subEntryIt++)
          {
            std::string subEntryDir = subEntryIt->name.GetString();

            Procedure(entryDir, subEntryDir);
          }
        }
      }

      for (auto entryIt = Entry.MemberBegin(); entryIt != Entry.MemberEnd(); entryIt++)
      {
        ImGui::PushID(&entryIt);

        std::string entryDir = entryIt->name.GetString();
        std::string entryName = entryIt->value["name"].GetString();

        if (entryName == "") entryName = entryDir;

        if (ImGui::BeginMenu(entryName.c_str()))
        {
          if (ImGui::Selectable("All"))
          {
            rj::Value& subEntries = entryIt->value["entries"];

            for (auto subEntryIt = subEntries.MemberBegin(); subEntryIt != subEntries.MemberEnd(); subEntryIt++)
            {
              std::string subEntryDir = subEntryIt->name.GetString();

              Procedure(entryDir, subEntryDir);
            }
          }

          rj::Value& subEntries = entryIt->value["entries"];

          for (auto subEntryIt = subEntries.MemberBegin(); subEntryIt != subEntries.MemberEnd(); subEntryIt++)
          {
            ImGui::PushID(&subEntryIt);

            std::string subEntryDir = subEntryIt->name.GetString();
            std::string subEntryName = subEntryIt->value["name"].GetString();

            if (subEntryName == "") subEntryName = subEntryDir;

            if (ImGui::Selectable(subEntryName.c_str()))
            {
              Procedure(entryDir, subEntryDir);
            }

            ImGui::PopID();
          }

          ImGui::EndMenu();
        }

        ImGui::PopID();
      }

      ImGui::EndMenu();
    }

    ImGui::PopID();
  }

  void MainMenu::DoProcFor(rj::Value& Entry, MenuItemProc Procedure)
  {
    for (auto entryIt = Entry.MemberBegin(); entryIt != Entry.MemberEnd(); entryIt++)
    {
      std::string entryDir = entryIt->name.GetString();

      rj::Value& subEntries = entryIt->value["entries"];

      for (auto subEntryIt = subEntries.MemberBegin(); subEntryIt != subEntries.MemberEnd(); subEntryIt++)
      {
        std::string subEntryDir = subEntryIt->name.GetString();

        Procedure(entryDir, subEntryDir);
      }
    }
  }
}
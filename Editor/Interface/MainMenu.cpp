#include <Common/Debug.h>

#include <Editor/Scene.h>
#include <Editor/Packer.h>
#include <Editor/Integrity.h>

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
    DrawIntegrityMenu();

    ImGui::EndMainMenuBar();
  }

  void MainMenu::DrawMapMenu()
  {
    if (ImGui::BeginMenu("Scene"))
    {
      DrawMenuTree("Regions", gArchive["regions"], Scene::Switch);

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
          DoProcFor(gArchive["regions"], Packer::DecryptArchive);
          DoProcFor(gArchive["characters"], Packer::DecryptArchive);
          DoProcFor(gArchive["items"], Packer::DecryptArchive);
          DoProcFor(gArchive["unknown"], Packer::DecryptArchive);
        }

        ImGui::Separator();

        DrawMenuTree("Regions", gArchive["regions"], Packer::DecryptArchive);
        DrawMenuTree("Characters", gArchive["characters"], Packer::DecryptArchive);
        DrawMenuTree("Items", gArchive["items"], Packer::DecryptArchive);
        DrawMenuTree("Unknown", gArchive["unknown"], Packer::DecryptArchive);

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Unpack"))
      {
        if (ImGui::Selectable("All"))
        {
          DoProcFor(gArchive["regions"], Packer::UnpackArchive);
          DoProcFor(gArchive["characters"], Packer::UnpackArchive);
          DoProcFor(gArchive["items"], Packer::UnpackArchive);
          DoProcFor(gArchive["unknown"], Packer::UnpackArchive);
        }

        ImGui::Separator();

        DrawMenuTree("Regions", gArchive["regions"], Packer::UnpackArchive);
        DrawMenuTree("Characters", gArchive["characters"], Packer::UnpackArchive);
        DrawMenuTree("Items", gArchive["items"], Packer::UnpackArchive);
        DrawMenuTree("Unknown", gArchive["unknown"], Packer::UnpackArchive);

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Repack"))
      {
        if (ImGui::Selectable("All"))
        {
          DoProcFor(gArchive["regions"], Packer::RepackArchive);
          DoProcFor(gArchive["characters"], Packer::RepackArchive);
          DoProcFor(gArchive["items"], Packer::RepackArchive);
          DoProcFor(gArchive["unknown"], Packer::RepackArchive);
        }

        ImGui::Separator();

        DrawMenuTree("Regions", gArchive["regions"], Packer::RepackArchive);
        DrawMenuTree("Characters", gArchive["characters"], Packer::RepackArchive);
        DrawMenuTree("Items", gArchive["items"], Packer::RepackArchive);
        DrawMenuTree("Unknown", gArchive["unknown"], Packer::RepackArchive);

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Encrypt"))
      {
        if (ImGui::Selectable("All"))
        {
          DoProcFor(gArchive["regions"], Packer::EncryptArchive);
          DoProcFor(gArchive["characters"], Packer::EncryptArchive);
          DoProcFor(gArchive["items"], Packer::EncryptArchive);
          DoProcFor(gArchive["unknown"], Packer::EncryptArchive);
        }

        ImGui::Separator();

        DrawMenuTree("Regions", gArchive["regions"], Packer::EncryptArchive);
        DrawMenuTree("Characters", gArchive["characters"], Packer::EncryptArchive);
        DrawMenuTree("Items", gArchive["items"], Packer::EncryptArchive);
        DrawMenuTree("Unknown", gArchive["unknown"], Packer::EncryptArchive);

        ImGui::EndMenu();
      }

      ImGui::EndMenu();
    }
  }

  void MainMenu::DrawIntegrityMenu()
  {
    if (ImGui::BeginMenu("Integrity"))
    {
      if (ImGui::Selectable("Check Encrypted")) Integrity::CheckEncrypted();
      if (ImGui::Selectable("Check Decrypted")) Integrity::CheckDecrypted();

      ImGui::Separator();

      if (ImGui::BeginMenu("Compare Repacked With Decrypted"))
      {
        if (ImGui::Selectable("All"))
        {
          DoProcFor(gArchive["regions"], Integrity::CompareRepackedWithDecrypted);
          DoProcFor(gArchive["characters"], Integrity::CompareRepackedWithDecrypted);
          DoProcFor(gArchive["items"], Integrity::CompareRepackedWithDecrypted);
          DoProcFor(gArchive["unknown"], Integrity::CompareRepackedWithDecrypted);
        }

        ImGui::Separator();

        DrawMenuTree("Regions", gArchive["regions"], Integrity::CompareRepackedWithDecrypted);
        DrawMenuTree("Characters", gArchive["characters"], Integrity::CompareRepackedWithDecrypted);
        DrawMenuTree("Items", gArchive["items"], Integrity::CompareRepackedWithDecrypted);
        DrawMenuTree("Unknown", gArchive["unknown"], Integrity::CompareRepackedWithDecrypted);

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Compare Encrypted With Original"))
      {
        if (ImGui::Selectable("All"))
        {
          DoProcFor(gArchive["regions"], Integrity::CompareEncryptedWithOriginal);
          DoProcFor(gArchive["characters"], Integrity::CompareEncryptedWithOriginal);
          DoProcFor(gArchive["items"], Integrity::CompareEncryptedWithOriginal);
          DoProcFor(gArchive["unknown"], Integrity::CompareEncryptedWithOriginal);
        }

        ImGui::Separator();

        DrawMenuTree("Regions", gArchive["regions"], Integrity::CompareEncryptedWithOriginal);
        DrawMenuTree("Characters", gArchive["characters"], Integrity::CompareEncryptedWithOriginal);
        DrawMenuTree("Items", gArchive["items"], Integrity::CompareEncryptedWithOriginal);
        DrawMenuTree("Unknown", gArchive["unknown"], Integrity::CompareEncryptedWithOriginal);

        ImGui::EndMenu();
      }

      ImGui::Separator();

      if (ImGui::Selectable("Generated Encrypted Map")) Integrity::GenerateEncryptedMap();
      if (ImGui::Selectable("Generated Decrypted Map")) Integrity::GenerateDecryptedMap();

      ImGui::EndMenu();
    }
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

      ImGui::Separator();

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

          ImGui::Separator();

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
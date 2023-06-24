#include <Common/Debug.h>

#include <Editor/Packer.h>
#include <Editor/Integrity.h>
#include <Editor/SceneManager.h>

#include <Editor/Interface/MainMenu.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern rj::Document gArchive;

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
      DrawMenuTree("Entities", gArchive["entities"], SceneManager::CreateEntity);
      DrawMenuTree("Regions", gArchive["regions"], SceneManager::CreateLevel);

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
          DoProcFor(gArchive["entities"], Packer::DecryptArchive);
        }

        ImGui::Separator();

        DrawMenuTree("Regions", gArchive["regions"], Packer::DecryptArchive);
        DrawMenuTree("Entities", gArchive["entities"], Packer::DecryptArchive);

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Unpack"))
      {
        if (ImGui::Selectable("All"))
        {
          DoProcFor(gArchive["regions"], Packer::UnpackArchive);
          DoProcFor(gArchive["entities"], Packer::UnpackArchive);
        }

        ImGui::Separator();

        DrawMenuTree("Regions", gArchive["regions"], Packer::UnpackArchive);
        DrawMenuTree("Entities", gArchive["entities"], Packer::UnpackArchive);

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Repack"))
      {
        if (ImGui::Selectable("All"))
        {
          DoProcFor(gArchive["regions"], Packer::RepackArchive);
          DoProcFor(gArchive["entities"], Packer::RepackArchive);
        }

        ImGui::Separator();

        DrawMenuTree("Regions", gArchive["regions"], Packer::RepackArchive);
        DrawMenuTree("Entities", gArchive["entities"], Packer::RepackArchive);

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Encrypt"))
      {
        if (ImGui::Selectable("All"))
        {
          DoProcFor(gArchive["regions"], Packer::EncryptArchive);
          DoProcFor(gArchive["entities"], Packer::EncryptArchive);
        }

        ImGui::Separator();

        DrawMenuTree("Regions", gArchive["regions"], Packer::EncryptArchive);
        DrawMenuTree("Entities", gArchive["entities"], Packer::EncryptArchive);

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
          DoProcFor(gArchive["entities"], Integrity::CompareRepackedWithDecrypted);
        }

        ImGui::Separator();

        DrawMenuTree("Regions", gArchive["regions"], Integrity::CompareRepackedWithDecrypted);
        DrawMenuTree("Entities", gArchive["entities"], Integrity::CompareRepackedWithDecrypted);

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Compare Encrypted With Original"))
      {
        if (ImGui::Selectable("All"))
        {
          DoProcFor(gArchive["regions"], Integrity::CompareEncryptedWithOriginal);
          DoProcFor(gArchive["entities"], Integrity::CompareEncryptedWithOriginal);
        }

        ImGui::Separator();

        DrawMenuTree("Regions", gArchive["regions"], Integrity::CompareEncryptedWithOriginal);
        DrawMenuTree("Entities", gArchive["entities"], Integrity::CompareEncryptedWithOriginal);

        ImGui::EndMenu();
      }

      ImGui::Separator();

      if (ImGui::Selectable("Generated Encrypted Map")) Integrity::GenerateEncryptedMap();
      if (ImGui::Selectable("Generated Decrypted Map")) Integrity::GenerateDecryptedMap();

      ImGui::EndMenu();
    }
  }
}
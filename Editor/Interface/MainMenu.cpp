#include <Common/Debug.h>

#include <Editor/Editor.h>
#include <Editor/Packer.h>
#include <Editor/Integrity.h>
#include <Editor/SceneManager.h>
#include <Editor/Tools.h>

#include <Editor/Interface/MainMenu.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	void MainMenu::Draw()
	{
		ImGui::BeginMainMenuBar();

		DrawSceneMenu();
		DrawPackerMenu();
		DrawIntegrityMenu();
		DrawToolsMenu();

		ImGui::EndMainMenuBar();
	}

	void MainMenu::DrawSceneMenu()
	{
		if (ImGui::BeginMenu("Scene"))
		{
			DrawMenuTree("Levels", gArchive["regions"], SceneManager::CreateLevel);
			DrawMenuTree("Entities", gArchive["entities"], SceneManager::CreateEntity);

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

			if (ImGui::Selectable("Generate Encrypted Map")) Integrity::GenerateEncryptedMap();
			if (ImGui::Selectable("Generate Decrypted Map")) Integrity::GenerateDecryptedMap();

			ImGui::EndMenu();
		}
	}

	void MainMenu::DrawToolsMenu()
	{
		if (ImGui::BeginMenu("Tools"))
		{
			DrawMenuTree("Generate Entity Thumbnails", gArchive["entities"], Tools::GenerateEntityThumbnail);

			ImGui::EndMenu();
		}
	}
}
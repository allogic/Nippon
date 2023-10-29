#pragma once

#include <vector>
#include <filesystem>
#include <utility>
#include <sstream>

#include <Common/Types.h>

#include <Editor/Interface.h>

#include <Editor/Databases/FileDatabase.h>

#include <Editor/ImGui/imgui.h>

namespace ark
{
	namespace fs = std::filesystem;

	class FileContainer;

	class MainMenu : public Interface
	{
	public:

		MainMenu();
		virtual ~MainMenu();

	public:

		virtual void Reset() override;
		virtual void Render() override;

	private:

		void RenderSceneMenu();
		void RenderArchiveMenu();
		void RenderToolsMenu();

	private:

		static void GenerateImGuiConfig();

	private:

		static void OpenSceneProcedure(const FileContainer* FileContainer);

		static void PrintTableOfContentProcedure(const FileContainer* FileContainer);
		static void ExtractToDiskProcedure(const FileContainer* FileContainer);

	private:

		template<typename P, typename ... Args>
		void RenderMenuWithProcedure(P Procedure, Args && ... Arguments)
		{
			if (ImGui::Selectable("All"))
			{
				for (const auto& directory : FileDatabase::GetDirectories())
				{
					for (const auto& fileContainer : FileDatabase::GetFileContainersByDirectory(directory))
					{
						Procedure(fileContainer, std::forward<Args>(Arguments) ...);
					}
				}
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("Levels"))
			{
				if (ImGui::Selectable("All"))
				{
					for (const auto& directory : FileDatabase::GetLevelDirectories())
					{
						for (const auto& fileContainer : FileDatabase::GetLevelFileContainersByDirectory(directory))
						{
							Procedure(fileContainer, std::forward<Args>(Arguments) ...);
						}
					}
				}

				ImGui::Separator();

				for (const auto& directory : FileDatabase::GetLevelDirectories())
				{
					ImGui::PushID(directory);

					if (ImGui::BeginMenu(FileDatabase::GetDirectoryNameByDirectory(directory)))
					{
						if (ImGui::Selectable("All"))
						{
							for (const auto& fileContainer : FileDatabase::GetLevelFileContainersByDirectory(directory))
							{
								Procedure(fileContainer, std::forward<Args>(Arguments) ...);
							}
						}

						ImGui::Separator();

						for (const auto& fileContainer : FileDatabase::GetLevelFileContainersByDirectory(directory))
						{
							ImGui::PushID(fileContainer);

							if (ImGui::Selectable(fileContainer->GetWindowName()))
							{
								Procedure(fileContainer, std::forward<Args>(Arguments) ...);
							}

							ImGui::PopID();
						}

						ImGui::EndMenu();
					}

					ImGui::PopID();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Entities"))
			{
				if (ImGui::Selectable("All"))
				{
					for (const auto& directory : FileDatabase::GetEntityDirectories())
					{
						for (const auto& fileContainer : FileDatabase::GetEntityFileContainersByDirectory(directory))
						{
							Procedure(fileContainer, std::forward<Args>(Arguments) ...);
						}
					}
				}

				ImGui::Separator();

				for (const auto& directory : FileDatabase::GetEntityDirectories())
				{
					ImGui::PushID(directory);

					if (ImGui::BeginMenu(FileDatabase::GetDirectoryNameByDirectory(directory)))
					{
						if (ImGui::Selectable("All"))
						{
							for (const auto& fileContainer : FileDatabase::GetEntityFileContainersByDirectory(directory))
							{
								Procedure(fileContainer, std::forward<Args>(Arguments) ...);
							}
						}

						ImGui::Separator();

						for (const auto& fileContainer : FileDatabase::GetEntityFileContainersByDirectory(directory))
						{
							ImGui::PushID(fileContainer);

							if (ImGui::Selectable(fileContainer->GetWindowName()))
							{
								Procedure(fileContainer, std::forward<Args>(Arguments) ...);
							}

							ImGui::PopID();
						}

						ImGui::EndMenu();
					}

					ImGui::PopID();
				}

				ImGui::EndMenu();
			}
		}
	};
}

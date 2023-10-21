#pragma once

#include <vector>
#include <filesystem>
#include <utility>

#include <Common/Types.h>

#include <Common/Generated/SceneInfos.h>

#include <Editor/Interface.h>

#include <Editor/ImGui/imgui.h>

namespace ark
{
	namespace fs = std::filesystem;

	class MainMenu : public Interface
	{
	public:

		MainMenu();
		virtual ~MainMenu();

	public:

		virtual void Reset() override;
		virtual void Render() override;

	private:

		void RenderFileMenu();
		void RenderEditMenu();
		void RenderSceneMenu();
		void RenderArchiveMenu();
		void RenderToolsMenu();

	private:

		static void DebugProcedure(const SceneInfo* SceneInfo);

		static void OpenSceneProcedure(const SceneInfo* SceneInfo);

		static void CopyAndDecryptOriginalToScratchProcedure(const SceneInfo* SceneInfo);
		static void LoadAndSaveFromScratchToScratchProcedure(const SceneInfo* SceneInfo);
		static void EncryptAndCopyScratchToFinishedProcedure(const SceneInfo* SceneInfo);
		static void CopyScratchToFinishedProcedure(const SceneInfo* SceneInfo);
		static void CompareScratchWithFinishedProcedure(const SceneInfo* SceneInfo);

		static void PrintTableOfContentProcedure(const SceneInfo* SceneInfo);
		static void ExtractToDiskProcedure(const SceneInfo* SceneInfo);

		static void CreateThumbnailProcedure(const SceneInfo* SceneInfo);

	private:

		template<typename P, typename ... Args>
		void RenderMenuWithProcedure(P Procedure, Args && ... Arguments)
		{
			if (ImGui::Selectable("All"))
			{
				for (const auto& groupInfo : SceneInfos::GetLevelGroups())
				{
					for (const auto& sceneInfo : SceneInfos::GetLevelsByGroup(groupInfo))
					{
						Procedure(&sceneInfo, std::forward<Args>(Arguments) ...);
					}
				}

				for (const auto& groupInfo : SceneInfos::GetEntityGroups())
				{
					for (const auto& sceneInfo : SceneInfos::GetEntitiesByGroup(groupInfo))
					{
						Procedure(&sceneInfo, std::forward<Args>(Arguments) ...);
					}
				}
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("Levels"))
			{
				if (ImGui::Selectable("All"))
				{
					for (const auto& groupInfo : SceneInfos::GetLevelGroups())
					{
						for (const auto& sceneInfo : SceneInfos::GetLevelsByGroup(groupInfo))
						{
							Procedure(&sceneInfo, std::forward<Args>(Arguments) ...);
						}
					}
				}

				ImGui::Separator();

				for (const auto& groupInfo : SceneInfos::GetLevelGroups())
				{
					ImGui::PushID(&groupInfo);

					if (ImGui::BeginMenu(groupInfo.MenuName.c_str()))
					{
						if (ImGui::Selectable("All"))
						{
							for (const auto& sceneInfo : SceneInfos::GetLevelsByGroup(groupInfo))
							{
								Procedure(&sceneInfo, std::forward<Args>(Arguments) ...);
							}
						}

						ImGui::Separator();

						for (const auto& sceneInfo : SceneInfos::GetLevelsByGroup(groupInfo))
						{
							ImGui::PushID(&sceneInfo);

							if (ImGui::Selectable(sceneInfo.MenuName.c_str()))
							{
								Procedure(&sceneInfo, std::forward<Args>(Arguments) ...);
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
					for (const auto& groupInfo : SceneInfos::GetEntityGroups())
					{
						for (const auto& sceneInfo : SceneInfos::GetEntitiesByGroup(groupInfo))
						{
							Procedure(&sceneInfo, std::forward<Args>(Arguments) ...);
						}
					}
				}

				ImGui::Separator();

				for (const auto& groupInfo : SceneInfos::GetEntityGroups())
				{
					ImGui::PushID(&groupInfo);

					if (ImGui::BeginMenu(groupInfo.MenuName.c_str()))
					{
						if (ImGui::Selectable("All"))
						{
							for (const auto& sceneInfo : SceneInfos::GetEntitiesByGroup(groupInfo))
							{
								Procedure(&sceneInfo, std::forward<Args>(Arguments) ...);
							}
						}

						ImGui::Separator();

						for (const auto& sceneInfo : SceneInfos::GetEntitiesByGroup(groupInfo))
						{
							ImGui::PushID(&sceneInfo);

							if (ImGui::Selectable(sceneInfo.MenuName.c_str()))
							{
								Procedure(&sceneInfo, std::forward<Args>(Arguments) ...);
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

		template<typename P, typename ... Args>
		void ExecuteLevelByGroupKeyProcedure(const std::string GroupKey, P Procedure, Args && ... Arguments)
		{
			for (const auto& sceneInfo : SceneInfos::GetLevelsByGroup(*SceneInfos::GetLevelGroupByKey(GroupKey)))
			{
				Procedure(&sceneInfo, std::forward<Args>(Arguments) ...);
			}
		}

		template<typename P, typename ... Args>
		void ExecuteEntityByGroupKeyProcedure(const std::string GroupKey, P Procedure, Args && ... Arguments)
		{
			for (const auto& sceneInfo : SceneInfos::GetEntitiesByGroup(*SceneInfos::GetEntityGroupByKey(GroupKey)))
			{
				Procedure(&sceneInfo, std::forward<Args>(Arguments) ...);
			}
		}
	};
}

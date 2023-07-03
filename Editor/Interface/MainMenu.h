#pragma once

#include <string>
#include <vector>
#include <utility>

#include <Common/Types.h>

#include <Vendor/ImGui/imgui.h>

#include <Vendor/rapidjson/rapidjson.h>
#include <Vendor/rapidjson/document.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace rj = rapidjson;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
	class MainMenu
	{
	public:

		void Draw();

	private:

		void DrawSceneMenu();
		void DrawPackerMenu();
		void DrawIntegrityMenu();
		void DrawToolsMenu();

	private:

		template<typename Proc, typename ... Args>
		static void DrawMenuTree(const std::string& Name, rj::Value& Entry, Proc Procedure, Args&& ... Arguments);

		template<typename Proc, typename ... Args>
		static void DoProcFor(rj::Value& Entry, Proc Procedure, Args&& ... Arguments);
	};
}

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	template<typename Proc, typename ... Args>
	void MainMenu::DrawMenuTree(const std::string& Name, rj::Value& Entry, Proc Procedure, Args&& ... Arguments)
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

						Procedure(entryDir, subEntryDir, subEntryIt->value, std::forward<Args>(Arguments) ...);
					}
				}
			}

			ImGui::Separator();

			for (auto entryIt = Entry.MemberBegin(); entryIt != Entry.MemberEnd(); entryIt++)
			{
				ImGui::PushID(entryIt->name.GetString());

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

							Procedure(entryDir, subEntryDir, subEntryIt->value, std::forward<Args>(Arguments) ...);
						}
					}

					ImGui::Separator();

					rj::Value& subEntries = entryIt->value["entries"];

					for (auto subEntryIt = subEntries.MemberBegin(); subEntryIt != subEntries.MemberEnd(); subEntryIt++)
					{
						ImGui::PushID(subEntryIt->name.GetString());

						std::string subEntryDir = subEntryIt->name.GetString();
						std::string subEntryName = subEntryIt->value["name"].GetString();

						if (subEntryName == "") subEntryName = subEntryDir;

						if (ImGui::Selectable(subEntryName.c_str()))
						{
							Procedure(entryDir, subEntryDir, subEntryIt->value, std::forward<Args>(Arguments) ...);
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

	template<typename Proc, typename ... Args>
	void MainMenu::DoProcFor(rj::Value& Entry, Proc Procedure, Args&& ... Arguments)
	{
		for (auto entryIt = Entry.MemberBegin(); entryIt != Entry.MemberEnd(); entryIt++)
		{
			std::string entryDir = entryIt->name.GetString();

			rj::Value& subEntries = entryIt->value["entries"];

			for (auto subEntryIt = subEntries.MemberBegin(); subEntryIt != subEntries.MemberEnd(); subEntryIt++)
			{
				std::string subEntryDir = subEntryIt->name.GetString();

				Procedure(entryDir, subEntryDir, subEntryIt->value, std::forward<Args>(Arguments) ...);
			}
		}
	}
}
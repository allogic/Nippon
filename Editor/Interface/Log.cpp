#include <Editor/Font/MaterialDesignIcons.h>

#include <Editor/Interface/Log.h>

#include <Editor/ImGui/imgui.h>

namespace Nippon
{
	void Log::Reset()
	{
		sBuffer.clear();

		sLineCount = 0;
	}

	void Log::Render()
	{
		auto lock = std::lock_guard<std::mutex>{ sMutex };

		ImGui::Begin(ICON_MDI_TEXT_LONG " Log");

		if (ImGui::Button("Clear"))
		{
			sBuffer.clear();

			sLineCount = 0;
		}

		ImGui::SameLine();

		static char buffer[32] = "1024";

		ImGui::PushItemWidth(100.0F);
		if (ImGui::InputText("Lines", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal))
		{
			sBuffer.clear();

			sMaxLineCount = std::strtoul(buffer, 0, 10);
			sLineCount = 0;
		}
		ImGui::PopItemWidth();

		ImGui::InputTextMultiline("##LogOutput", sBuffer.data(), sBuffer.size(), ImGui::GetContentRegionAvail(), ImGuiInputTextFlags_ReadOnly);

		ImGui::End();
	}
}
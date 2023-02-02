#include <Editor/Scene.h>

#include <Editor/Interface/Scene/ObjectBrowser.h>

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
  void ObjectBrowser::Draw()
  {
    ImGui::Begin("Object Browser");

    if (ImGui::BeginTable("Object Table", 13, ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersInnerV))
      {
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 15.0F);
        ImGui::TableSetupColumn("CA", ImGuiTableColumnFlags_WidthFixed, 15.0F);
        ImGui::TableSetupColumn("U3", ImGuiTableColumnFlags_WidthFixed, 15.0F);
        ImGui::TableSetupColumn("U4", ImGuiTableColumnFlags_WidthFixed, 15.0F);
        ImGui::TableSetupColumn("Scale", ImGuiTableColumnFlags_WidthFixed, 90.0F);
        ImGui::TableSetupColumn("Rotation", ImGuiTableColumnFlags_WidthFixed, 90.0F);
        ImGui::TableSetupColumn("Position", ImGuiTableColumnFlags_WidthFixed, 132.5F);
        ImGui::TableSetupColumn("U5", ImGuiTableColumnFlags_WidthFixed, 55.0F);
        ImGui::TableSetupColumn("U6", ImGuiTableColumnFlags_WidthFixed, 55.0F);
        ImGui::TableSetupColumn("U7", ImGuiTableColumnFlags_WidthFixed, 15.0F);
        ImGui::TableSetupColumn("U8", ImGuiTableColumnFlags_WidthFixed, 15.0F);
        ImGui::TableSetupColumn("U9", ImGuiTableColumnFlags_WidthFixed, 15.0F);
        ImGui::TableSetupColumn("UA", ImGuiTableColumnFlags_WidthFixed, 15.0F);
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        if (gScene)
        {
          for (const auto& object : gScene->GetObjects())
          {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            ImGui::Text("%02X", object.Id);
            ImGui::TableNextColumn();

            ImGui::Text("%02X", object.Category);
            ImGui::TableNextColumn();

            ImGui::Text("%02X", object.Unknown3);
            ImGui::TableNextColumn();

            ImGui::Text("%02X", object.Unknown4);
            ImGui::TableNextColumn();

            ImGui::Text("(%3u,%3u,%3u)", object.Scale.x, object.Scale.y, object.Scale.z);
            ImGui::TableNextColumn();

            ImGui::Text("(%3u,%3u,%3u)", object.Rotation.x, object.Rotation.y, object.Rotation.z);
            ImGui::TableNextColumn();

            ImGui::Text("(%5u,%5u,%5u)", object.Position.x, object.Position.y, object.Position.z);
            ImGui::TableNextColumn();

            ImGui::Text("%08X", object.Unknown5);
            ImGui::TableNextColumn();

            ImGui::Text("%08X", object.Unknown6);
            ImGui::TableNextColumn();

            ImGui::Text("%02X", object.Unknown7);
            ImGui::TableNextColumn();

            ImGui::Text("%02X", object.Unknown8);
            ImGui::TableNextColumn();

            ImGui::Text("%02X", object.Unknown9);
            ImGui::TableNextColumn();

            ImGui::Text("%02X", object.UnknownA);
            ImGui::TableNextColumn();
          }
        }

        ImGui::EndTable();
      }

    ImGui::End();
  }
}
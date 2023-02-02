#include <Editor/Scene.h>

#include <Editor/Interface/ModelBrowser.h>

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
  void ModelBrowser::Draw()
  {
    ImGui::Begin("Model Browser");

    if (ImGui::BeginTable("Model Table", 7, ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersInnerV))
    {
      ImGui::TableSetupColumn("Header", ImGuiTableColumnFlags_WidthFixed, 55.0F);
      ImGui::TableSetupColumn("MT", ImGuiTableColumnFlags_WidthFixed, 15.0F);
      ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 35.0F);
      ImGui::TableSetupColumn("Div", ImGuiTableColumnFlags_WidthFixed, 35.0F);
      ImGui::TableSetupColumn("Scale", ImGuiTableColumnFlags_WidthFixed, 132.5F);
      ImGui::TableSetupColumn("Rotation", ImGuiTableColumnFlags_WidthFixed, 152.5F);
      ImGui::TableSetupColumn("Position", ImGuiTableColumnFlags_WidthFixed, 152.5F);
      ImGui::TableSetupScrollFreeze(0, 1);
      ImGui::TableHeadersRow();

      if (gScene)
      {
        for (const auto& [model, transform] : gScene->GetModels())
        {
          ImGui::TableNextRow();
          ImGui::TableNextColumn();

          ImGui::Text("%08X", model.Header.MdbId);
          ImGui::TableNextColumn();

          ImGui::Text("%02X", model.Header.MeshType);
          ImGui::TableNextColumn();

          ImGui::Text("%5u", model.Header.MeshId);
          ImGui::TableNextColumn();

          ImGui::Text("%5u", model.Header.MeshDivisions);
          ImGui::TableNextColumn();

          ImGui::Text("(%5u,%5u,%5u)", transform.Scale.x, transform.Scale.y, transform.Scale.z);
          ImGui::TableNextColumn();

          ImGui::Text("(%6d,%6d,%6d)", transform.Rotation.x, transform.Rotation.y, transform.Rotation.z);
          ImGui::TableNextColumn();

          ImGui::Text("(%6d,%6d,%6d)", transform.Position.x, transform.Position.y, transform.Position.z);
          ImGui::TableNextColumn();
        }
      }

      ImGui::EndTable();
    }

    ImGui::End();
  }
}
#include <Editor/UI/AssetDatabase.h>

#include <Vendor/ImGui/imgui.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  void AssetDatabase::Update()
  {

  }

  void AssetDatabase::Draw()
  {
    ImGui::Begin("Object Database");

    if (ImGui::BeginTable("Object Table", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersInnerV))
    {
      ImGui::TableSetupColumn("Id", ImGuiTableColumnFlags_WidthFixed, 100.0F);
      ImGui::TableSetupColumn("Category", ImGuiTableColumnFlags_WidthFixed, 100.0F);
      ImGui::TableSetupColumn("Transform", ImGuiTableColumnFlags_WidthFixed, 500.0F);
      ImGui::TableSetupScrollFreeze(0, 1);
      ImGui::TableHeadersRow();

      for (const auto& object : mObjects)
      {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        ImGui::Text("%u", object.GetId());
        ImGui::TableNextColumn();

        ImGui::Text("%u", object.GetCategory());
        ImGui::TableNextColumn();

        ImGui::Text("[%6u,%6u,%6u] [%6u,%6u,%6u] [%6u,%6u,%6u]",
          object.GetPosition().x,
          object.GetPosition().y,
          object.GetPosition().z,
          object.GetRotation().x,
          object.GetRotation().y,
          object.GetRotation().z,
          object.GetScale().x,
          object.GetScale().y,
          object.GetScale().z);
        ImGui::TableNextColumn();
      }

      ImGui::EndTable();
    }

    ImGui::End();

    ImGui::Begin("Model Database");

    if (ImGui::BeginTable("Model Table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersInnerV))
    {
      ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 150.0F);
      ImGui::TableSetupColumn("Transform", ImGuiTableColumnFlags_WidthFixed, 500.0F);
      ImGui::TableSetupColumn("Vertex Count", ImGuiTableColumnFlags_WidthFixed, 100.0F);
      ImGui::TableSetupColumn("Element Count", ImGuiTableColumnFlags_WidthStretch, 100.0F);
      ImGui::TableSetupScrollFreeze(0, 1);
      ImGui::TableHeadersRow();

      for (const auto& model : mModels)
      {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        ImGui::Text("%s", model.GetName().c_str());
        ImGui::TableNextColumn();

        ImGui::Text("[%6d,%6d,%6d] [%6d,%6d,%6d] [%6u,%6u,%6u]",
          model.GetTransform().Position.x,
          model.GetTransform().Position.y,
          model.GetTransform().Position.z,
          model.GetTransform().Rotation.x,
          model.GetTransform().Rotation.y,
          model.GetTransform().Rotation.z,
          model.GetTransform().Scale.x,
          model.GetTransform().Scale.y,
          model.GetTransform().Scale.z);
        ImGui::TableNextColumn();

        ImGui::Text("%u", model.GetVertexBuffer().size());
        ImGui::TableNextColumn();

        ImGui::Text("%u", model.GetElementBuffer().size());
        ImGui::TableNextColumn();
      }

      ImGui::EndTable();
    }

    ImGui::End();
  }
}
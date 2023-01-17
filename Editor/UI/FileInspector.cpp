#include <Editor/UI/FileInspector.h>

#include <Vendor/ImGui/imgui.h>

#include <Vendor/rapidjson/document.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern rapidjson::Document gWorld;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	void FileInspector::Update()
	{
		delete mFileNode;
		mFileNode = new FileNode{ gWorld["unpackDir"].GetString() };
		mSelectedNode = nullptr;
	}

	void FileInspector::Draw()
	{
		ImGui::Begin("File Inspector");

		if (ImGui::Button("Update"))
		{
			Update();
		}

		DrawFileNodeRecursive(mFileNode);

		ImGui::End();
	}

	void FileInspector::DrawFileNodeRecursive(FileNode* Node)
	{
		std::uint32_t flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;

		if (Node == mSelectedNode) flags |= ImGuiTreeNodeFlags_Selected;

		if (Node->IsFile())
		{
			flags |= ImGuiTreeNodeFlags_Leaf;
		}

		std::string name = Node->GetName().string() + Node->GetExtension().string();
		std::uint32_t opened = ImGui::TreeNodeEx(name.c_str(), flags);

		if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1))
		{
			mSelectedNode = Node;
		}

		if (opened)
		{
			for (auto& [file, node] : *Node)
			{
				DrawFileNodeRecursive(node);
			}

			ImGui::TreePop();
		}
	}
}
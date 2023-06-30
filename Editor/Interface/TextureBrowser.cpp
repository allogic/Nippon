#include <Editor/Texture.h>
#include <Editor/Scene.h>
#include <Editor/SceneManager.h>

#include <Editor/Interface/TextureBrowser.h>

#include <Vendor/ImGui/imgui.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	void TextureBrowser::Reset()
	{

	}

	void TextureBrowser::Draw()
	{
		ImGui::Begin("Texture Browser");

		Scene* scene = SceneManager::GetActiveScene();

		if (scene)
		{      
			const auto& textures = scene->GetTextures();
			
			U32 size = (U32)textures.size();
			
			ImVec2 view = ImGui::GetContentRegionAvail();
			
			for (U32 i = 0; i < size; i++)
			{      
				R32 width = std::min(view.x, (R32)textures[i]->GetWidth());
				R32 height = std::min(view.y, (R32)textures[i]->GetHeight());
			
				ImGui::Image((void*)(U64)textures[i]->GetId(), ImVec2{ width, (R32)textures[i]->GetHeight() }, ImVec2{ 0.0F, 1.0F }, ImVec2{ 1.0F, 0.0F });
				ImGui::Separator();
			}
		}

		ImGui::End();
	}
}
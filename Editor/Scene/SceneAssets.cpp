#include <Common/Memory.h>

#include <Common/Archive/Archive.h>
#include <Common/Model/Model.h>

#include <Editor/OpenGl/Texture2D.h>

#include <Editor/Scene/SceneAssets.h>

#include <Editor/Utility/TextureUtility.h>

namespace Nippon
{
	SceneAssets::SceneAssets()
	{

	}

	SceneAssets::~SceneAssets()
	{
		if (mArchive)
		{
			mArchive->~Archive();

			Memory::Free(mArchive);
		}

		for (auto const& model : mModels)
		{
			model->~Model();

			Memory::Free(model);
		}

		for (auto const& texture : mTextures)
		{
			Texture2D::Destroy(texture);
		}
	}

	void SceneAssets::AddScrModelFromArchive(Archive* Archive)
	{
		Model* model = new (Memory::Alloc(sizeof(Model))) Model;

		mModels.emplace_back(model);

		model->Deserialize(Archive->GetBytes(), Archive->GetSize());
		model->SetName(Archive->GetName());
	}

	void SceneAssets::AddMdModelFromArchive(Archive* Archive, Object const* Object)
	{
		Model* model = new (Memory::Alloc(sizeof(Model))) Model;

		mModels.emplace_back(model);

		model->Deserialize(Archive->GetBytes(), Archive->GetSize());
		model->SetName(Archive->GetName());
		model->SetObjectRef(Object);
	}

	void SceneAssets::AddTextureFromArchive(Archive* Archive)
	{
		mTextures.emplace_back(TextureUtility::ReadDDS(Archive->GetBytes(), Archive->GetSize()));
	}
}
#include <Memory.h>

#include <Archive/Archive.h>
#include <Archive/Model.h>
#include <Archive/ModelTypes.h>

#include <Archive/Serializer/MeshSerializer.h>

#include <OpenGl/Texture2D.h>

#include <Scene/SceneAssets.h>

#include <Utility/TextureUtility.h>

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

		for (auto const& texture : mTextures)
		{
			Texture2D::Destroy(texture);
		}
	}

	void SceneAssets::AddScrModelFromArchive(Archive* Archive)
	{
		Model& model = mModels.emplace_back();

		model.SetType(ModelType::eModelTypeScr);
		model.SetName(Archive->GetName());
		model.SetScrMeshes(MeshSerializer::ScrMeshesFromBytes(Archive->GetBytes(), Archive->GetSize()));
	}

	void SceneAssets::AddMdModelFromArchive(Archive* Archive, Object const* Object)
	{
		Model& model = mModels.emplace_back();

		model.SetType(ModelType::eModelTypeMd);
		model.SetName(Archive->GetName());
		model.SetMdMeshes(MeshSerializer::MdMeshesFromBytes(Archive->GetBytes(), Archive->GetSize()));
		model.SetObjectRef(Object);
	}

	void SceneAssets::AddTextureFromArchive(Archive* Archive)
	{
		mTextures.emplace_back(TextureUtility::ReadDDS(Archive->GetBytes(), Archive->GetSize()));
	}
}
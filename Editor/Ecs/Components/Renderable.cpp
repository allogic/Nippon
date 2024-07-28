#include <Common/Memory.h>

#include <Editor/Ecs/Components/Renderable.h>

#include <Editor/OpenGl/StaticMesh.h>
#include <Editor/OpenGl/MeshTypes.h>

namespace Nippon
{
	Renderable::Renderable(Scene* Scene, Entity* Entity) : Component(Scene, Entity)
	{
		mStaticMesh = new (Memory::Alloc(sizeof(StaticMesh))) StaticMesh{ MeshType::eMeshTypeDefault };
	}

	Renderable::~Renderable()
	{
		mStaticMesh->~StaticMesh();

		Memory::Free(mStaticMesh);
	}

	void Renderable::SetTexture(U32 TextureIndex, U32 Texture)
	{
		mTextureIndex = TextureIndex;
		mTexture = Texture;
	}
}
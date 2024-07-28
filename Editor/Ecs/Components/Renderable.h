#pragma once

#include <Common/Forward.h>
#include <Common/VertexTypes.h>

#include <Editor/Forward.h>

#include <Editor/Ecs/Component.h>

namespace Nippon
{
	class Renderable : public Component
	{
	public:

		Renderable(Scene* Scene, Entity* Entity);
		virtual ~Renderable();

	public:

		inline auto const& GetStaticMesh() const { return mStaticMesh; }
		inline auto const& GetTextureIndex() const { return mTextureIndex; }
		inline auto const& GetTexture() const { return mTexture; }

	public:

		void SetTexture(U32 TextureIndex, U32 Texture);

	private:

		StaticMesh* mStaticMesh = nullptr;

		U32 mTextureIndex = 0;
		U32 mTexture = 0;
	};
}
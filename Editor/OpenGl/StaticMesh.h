#pragma once

#include <Standard.h>
#include <Types.h>

#include <OpenGl/MeshTypes.h>
#include <OpenGl/VertexTypes.h>

#include <Renderer/RenderModes.h>

namespace Nippon
{
	class StaticMesh
	{
	public:

		StaticMesh(MeshType MeshType);
		virtual ~StaticMesh();

	public:

		inline auto const& GetVertexBufferCount() const { return mVertexBufferCount; }
		inline auto const& GetIndexBufferCount() const { return mIndexBufferCount; }

	public:

		void Bind();
		void Render(RenderMode RenderMode);
		void UnBind();

	public:

		void GetVertices(void* VertexBuffer, U64 VertexStructSize, U64 VertexBufferCount);
		void GetIndices(void* IndexBuffer, U64 IndexStructSize, U64 IndexBufferCount);

		void SetVertices(void* VertexBuffer, U64 VertexStructSize, U64 VertexBufferCount);
		void SetIndices(void* IndexBuffer, U64 IndexStructSize, U64 IndexBufferCount);

	private:

		U32 mVao = 0;
		U32 mVbo = 0;
		U32 mEbo = 0;

		U64 mVertexBufferCount = 0;
		U64 mIndexBufferCount = 0;
	};
}
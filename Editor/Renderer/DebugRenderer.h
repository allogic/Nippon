#pragma once

#include <filesystem>

#include <Common/Types.h>

#include <Editor/Forward.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace fs = std::filesystem;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
	class DebugRenderer
	{
	public:

		DebugRenderer(Scene* Scene, U32 VertexBufferSize, U32 ElementBufferSize);
		virtual ~DebugRenderer();

	public:

		void Render();

	public:

		void DebugLine(const R32V3& P0, const R32V3& P1, const R32V4& C);
		void DebugBox(const R32V3& P, const R32V3& S, const R32V4& C, const R32Q& R = {});
		void DebugAxisAlignedBoundingBox(const R32V3& P, const AABB& AABB, const R32V4& C);

	private:

		Scene* mScene;
		DebugVertex* mVertexBuffer;
		U32* mElementBuffer;
		Mesh<DebugVertex, U32>* mMesh;
		Shader* mShader;

		U32 mVertexOffset = 0;
		U32 mElementOffset = 0;
	};
}
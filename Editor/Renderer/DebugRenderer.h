#pragma once

#include <filesystem>

#include <Common/Types.h>

#include <Editor/Vertex.h>

namespace ark
{
	namespace fs = std::filesystem;

	class Shader;

	template<typename V, typename E>
	class Mesh;

	class DebugRenderer
	{
	public:

		DebugRenderer(U32 VertexBufferSize, U32 ElementBufferSize);
		virtual ~DebugRenderer();

	public:

		void Render();

	public:

		void DebugLine(const R32V3& P0, const R32V3& P1, const R32V4& C);
		void DebugBox(const R32V3& P, const R32V3& S, const R32V4& C, const R32Q& R = {});
		void DebugGrid(const R32V3& P, U32 N, R32 S, const R32V4& C, const R32Q& R = {});
		void DebugAABB(const R32V3& P, const AABB& AABB, const R32V4& C);

	private:

		DebugVertex* mVertexBuffer;
		U32* mElementBuffer;
		Mesh<DebugVertex, U32>* mMesh;
		Shader* mShader;

		U32 mVertexOffset = 0;
		U32 mElementOffset = 0;
	};
}

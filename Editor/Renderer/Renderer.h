#pragma once

#include <Forward.h>
#include <Standard.h>
#include <Types.h>

#include <Math/AABB.h>

#include <OpenGl/VertexTypes.h>

namespace Nippon
{
	struct RenderTask
	{
		Transform* Transform;
		Renderable* Renderable;
	};

	class Renderer
	{
	public:

		Renderer(Scene* Scene);
		virtual ~Renderer();

	public:

		void RenderDefault();
		void RenderDebug();

	public:

		void FlushDefaultQueue();
		void SubmitToDefaultQueue(Transform* Transform, Renderable* Renderable);

	public:

		void DebugLine(R32V3 const& P0, R32V3 const& P1, R32V4 const& C);
		void DebugBox(R32V3 const& P, R32V3 const& S, R32V4 const& C, R32Q const& R = {});
		void DebugGrid(R32V3 const& P, U32 N, R32 S, R32V4 const& C, R32Q const& R = {});
		void DebugAABB(R32V3 const& P, AABB const& AABB, R32V4 const& C);

	private:

		Scene* mScene = nullptr;

		StaticMesh* mDebugStaticMesh = nullptr;

		Shader* mDefaultMeshShader = nullptr;
		Shader* mDebugMeshShader = nullptr;

		DebugVertex* mDebugVertexBuffer = nullptr;
		U32* mDebugIndexBuffer = nullptr;

		U32 mDebugVertexBufferOffset = 0;
		U32 mDebugIndexBufferOffset = 0;

		std::vector<RenderTask> mDefaultRenderQueue = {};
	};
}
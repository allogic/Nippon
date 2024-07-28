#include <Common/Memory.h>

#include <Common/Math/AABB.h>

#include <Editor/Ecs/Registry.h>

#include <Editor/Ecs/Components/Camera.h>
#include <Editor/Ecs/Components/Renderable.h>
#include <Editor/Ecs/Components/Transform.h>

#include <Editor/OpenGl/MeshTypes.h>
#include <Editor/OpenGl/StaticMesh.h>
#include <Editor/OpenGl/Shader.h>
#include <Editor/OpenGl/Texture2D.h>

#include <Editor/Renderer/Renderer.h>
#include <Editor/Renderer/RenderModes.h>

#include <Editor/Scene/Scene.h>

#define DEBUG_VERTEX_BUFFER_SIZE 131070
#define DEBUG_INDEX_BUFFER_SIZE (DEBUG_VERTEX_BUFFER_SIZE * 2)

namespace Nippon
{
	Renderer::Renderer(Scene* Scene)
	{
		mScene = Scene;

		mDebugStaticMesh = new (Memory::Alloc(sizeof(StaticMesh))) StaticMesh{ MeshType::eMeshTypeDebug };

		mDefaultMeshShader = new (Memory::Alloc(sizeof(Shader))) Shader{ "DefaultVertexShader.glsl", "DefaultFragmentShader.glsl" };
		mDebugMeshShader = new (Memory::Alloc(sizeof(Shader))) Shader{ "DebugVertexShader.glsl", "DebugFragmentShader.glsl" };

		mDebugVertexBuffer = (DebugVertex*)Memory::Alloc(sizeof(DebugVertex) * DEBUG_VERTEX_BUFFER_SIZE);
		mDebugIndexBuffer = (U32*)Memory::Alloc(sizeof(U32) * DEBUG_INDEX_BUFFER_SIZE);
	}

	Renderer::~Renderer()
	{
		mDebugStaticMesh->~StaticMesh();

		mDefaultMeshShader->~Shader();
		mDebugMeshShader->~Shader();

		Memory::Free(mDebugStaticMesh);

		Memory::Free(mDefaultMeshShader);
		Memory::Free(mDebugMeshShader);

		Memory::Free(mDebugVertexBuffer);
		Memory::Free(mDebugIndexBuffer);
	}

	void Renderer::RenderDefault()
	{
		if (Registry* registry = mScene->GetRegistry())
		{
			if (Camera* camera = registry->GetMainCamera())
			{
				mDefaultMeshShader->Bind();

				mDefaultMeshShader->SetUniformR32M4("UniformProjectionMatrix", camera->GetProjectionMatrix());
				mDefaultMeshShader->SetUniformR32M4("UniformViewMatrix", camera->GetViewMatrix());

				for (auto const& renderTask : mDefaultRenderQueue)
				{
					Transform* transform = renderTask.Transform;
					Renderable* renderable = renderTask.Renderable;

					if (transform && renderable)
					{
						mDefaultMeshShader->SetUniformR32M4("UniformModelMatrix", transform->GetModelMatrix());

						StaticMesh* staticMesh = renderable->GetStaticMesh();
						U32 texture = renderable->GetTexture();

						Texture2D::Bind(texture);
						Texture2D::Mount(0, texture);

						staticMesh->Bind();
						staticMesh->Render(RenderMode::eRenderModeTriangles);
						staticMesh->UnBind();

						Texture2D::UnMount(0);
						Texture2D::UnBind();
					}
				}

				mDefaultMeshShader->UnBind();
			}
		}
	}

	void Renderer::RenderDebug()
	{
		if (Registry* registry = mScene->GetRegistry())
		{
			if (Camera* camera = registry->GetMainCamera())
			{
				mDebugMeshShader->Bind();
		
				mDebugMeshShader->SetUniformR32M4("UniformProjectionMatrix", camera->GetProjectionMatrix());
				mDebugMeshShader->SetUniformR32M4("UniformViewMatrix", camera->GetViewMatrix());
		
				mDebugStaticMesh->Bind();
				mDebugStaticMesh->SetVertices(mDebugVertexBuffer, sizeof(DebugVertex), mDebugVertexBufferOffset);
				mDebugStaticMesh->SetIndices(mDebugIndexBuffer, sizeof(U32), mDebugIndexBufferOffset);
				mDebugStaticMesh->Render(RenderMode::eRenderModeLines);
				mDebugStaticMesh->UnBind();
		
				mDebugMeshShader->UnBind();
			}
		}

		mDebugVertexBufferOffset = 0;
		mDebugIndexBufferOffset = 0;
	}

	void Renderer::FlushDefaultQueue()
	{
		mDefaultRenderQueue.clear();
	}

	void Renderer::SubmitToDefaultQueue(Transform* Transform, Renderable* Renderable)
	{
		mDefaultRenderQueue.emplace_back(RenderTask{ Transform, Renderable });
	}

	void Renderer::DebugLine(R32V3 const& P0, R32V3 const& P1, R32V4 const& C)
	{
		mDebugVertexBuffer[mDebugVertexBufferOffset + 0].Position = P0;
		mDebugVertexBuffer[mDebugVertexBufferOffset + 1].Position = P1;

		mDebugVertexBuffer[mDebugVertexBufferOffset + 0].Color = C;
		mDebugVertexBuffer[mDebugVertexBufferOffset + 1].Color = C;

		mDebugIndexBuffer[mDebugIndexBufferOffset + 0] = mDebugVertexBufferOffset + 0;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 1] = mDebugVertexBufferOffset + 1;

		mDebugVertexBufferOffset += 2;
		mDebugIndexBufferOffset += 2;
	}

	void Renderer::DebugBox(R32V3 const& P, R32V3 const& S, R32V4 const& C, R32Q const& R)
	{
		R32V3 h{ S / 2.0F };

		mDebugVertexBuffer[mDebugVertexBufferOffset + 0].Position = P + R * R32V3{ -h.x, -h.y, -h.z };
		mDebugVertexBuffer[mDebugVertexBufferOffset + 1].Position = P + R * R32V3{ h.x, -h.y, -h.z };
		mDebugVertexBuffer[mDebugVertexBufferOffset + 2].Position = P + R * R32V3{ -h.x, h.y, -h.z };
		mDebugVertexBuffer[mDebugVertexBufferOffset + 3].Position = P + R * R32V3{ h.x, h.y, -h.z };

		mDebugVertexBuffer[mDebugVertexBufferOffset + 4].Position = P + R * R32V3{ -h.x, -h.y, h.z };
		mDebugVertexBuffer[mDebugVertexBufferOffset + 5].Position = P + R * R32V3{ h.x, -h.y, h.z };
		mDebugVertexBuffer[mDebugVertexBufferOffset + 6].Position = P + R * R32V3{ -h.x, h.y, h.z };
		mDebugVertexBuffer[mDebugVertexBufferOffset + 7].Position = P + R * R32V3{ h.x, h.y, h.z };

		mDebugVertexBuffer[mDebugVertexBufferOffset + 0].Color = C;
		mDebugVertexBuffer[mDebugVertexBufferOffset + 1].Color = C;
		mDebugVertexBuffer[mDebugVertexBufferOffset + 2].Color = C;
		mDebugVertexBuffer[mDebugVertexBufferOffset + 3].Color = C;

		mDebugVertexBuffer[mDebugVertexBufferOffset + 4].Color = C;
		mDebugVertexBuffer[mDebugVertexBufferOffset + 5].Color = C;
		mDebugVertexBuffer[mDebugVertexBufferOffset + 6].Color = C;
		mDebugVertexBuffer[mDebugVertexBufferOffset + 7].Color = C;

		mDebugIndexBuffer[mDebugIndexBufferOffset + 0] = mDebugVertexBufferOffset + 0;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 1] = mDebugVertexBufferOffset + 1;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 2] = mDebugVertexBufferOffset + 0;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 3] = mDebugVertexBufferOffset + 2;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 4] = mDebugVertexBufferOffset + 2;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 5] = mDebugVertexBufferOffset + 3;

		mDebugIndexBuffer[mDebugIndexBufferOffset + 6] = mDebugVertexBufferOffset + 3;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 7] = mDebugVertexBufferOffset + 1;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 8] = mDebugVertexBufferOffset + 4;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 9] = mDebugVertexBufferOffset + 5;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 10] = mDebugVertexBufferOffset + 4;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 11] = mDebugVertexBufferOffset + 6;

		mDebugIndexBuffer[mDebugIndexBufferOffset + 12] = mDebugVertexBufferOffset + 6;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 13] = mDebugVertexBufferOffset + 7;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 14] = mDebugVertexBufferOffset + 7;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 15] = mDebugVertexBufferOffset + 5;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 16] = mDebugVertexBufferOffset + 0;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 17] = mDebugVertexBufferOffset + 4;

		mDebugIndexBuffer[mDebugIndexBufferOffset + 18] = mDebugVertexBufferOffset + 1;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 19] = mDebugVertexBufferOffset + 5;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 20] = mDebugVertexBufferOffset + 2;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 21] = mDebugVertexBufferOffset + 6;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 22] = mDebugVertexBufferOffset + 3;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 23] = mDebugVertexBufferOffset + 7;

		mDebugVertexBufferOffset += 8;
		mDebugIndexBufferOffset += 24;
	}

	void Renderer::DebugGrid(R32V3 const& P, U32 N, R32 S, R32V4 const& C, R32Q const& R)
	{
		U32 indexOffset = 0;

		R32 sizeStep = S / N;
		R32 halfSize = S / 2;

		for (U32 i = 0; i <= N; i++)
		{
			R32 gridOffset = (R32)i * sizeStep - halfSize;

			mDebugVertexBuffer[mDebugVertexBufferOffset + indexOffset + 0].Position = P + R * R32V3{ gridOffset, 0.0F, -halfSize };
			mDebugVertexBuffer[mDebugVertexBufferOffset + indexOffset + 1].Position = P + R * R32V3{ gridOffset, 0.0F, halfSize };
			mDebugVertexBuffer[mDebugVertexBufferOffset + indexOffset + 2].Position = P + R * R32V3{ -halfSize, 0.0F, gridOffset };
			mDebugVertexBuffer[mDebugVertexBufferOffset + indexOffset + 3].Position = P + R * R32V3{ halfSize, 0.0F, gridOffset };

			mDebugVertexBuffer[mDebugVertexBufferOffset + indexOffset + 0].Color = C;
			mDebugVertexBuffer[mDebugVertexBufferOffset + indexOffset + 1].Color = C;
			mDebugVertexBuffer[mDebugVertexBufferOffset + indexOffset + 2].Color = C;
			mDebugVertexBuffer[mDebugVertexBufferOffset + indexOffset + 3].Color = C;

			mDebugIndexBuffer[mDebugIndexBufferOffset + indexOffset + 0] = mDebugVertexBufferOffset + indexOffset + 0;
			mDebugIndexBuffer[mDebugIndexBufferOffset + indexOffset + 1] = mDebugVertexBufferOffset + indexOffset + 1;
			mDebugIndexBuffer[mDebugIndexBufferOffset + indexOffset + 2] = mDebugVertexBufferOffset + indexOffset + 2;
			mDebugIndexBuffer[mDebugIndexBufferOffset + indexOffset + 3] = mDebugVertexBufferOffset + indexOffset + 3;

			indexOffset += 4;
		}

		mDebugVertexBufferOffset += indexOffset;
		mDebugIndexBufferOffset += indexOffset;
	}

	void Renderer::DebugAABB(R32V3 const& P, AABB const& AABB, R32V4 const& C)
	{
		R32V3 const& min = AABB.GetMin();
		R32V3 const& max = AABB.GetMax();

		mDebugVertexBuffer[mDebugVertexBufferOffset + 0].Position = P + R32V3{ min.x, min.y, min.z };
		mDebugVertexBuffer[mDebugVertexBufferOffset + 1].Position = P + R32V3{ max.x, min.y, min.z };
		mDebugVertexBuffer[mDebugVertexBufferOffset + 2].Position = P + R32V3{ min.x, max.y, min.z };
		mDebugVertexBuffer[mDebugVertexBufferOffset + 3].Position = P + R32V3{ max.x, max.y, min.z };

		mDebugVertexBuffer[mDebugVertexBufferOffset + 4].Position = P + R32V3{ min.x, min.y, max.z };
		mDebugVertexBuffer[mDebugVertexBufferOffset + 5].Position = P + R32V3{ max.x, min.y, max.z };
		mDebugVertexBuffer[mDebugVertexBufferOffset + 6].Position = P + R32V3{ min.x, max.y, max.z };
		mDebugVertexBuffer[mDebugVertexBufferOffset + 7].Position = P + R32V3{ max.x, max.y, max.z };

		mDebugVertexBuffer[mDebugVertexBufferOffset + 0].Color = C;
		mDebugVertexBuffer[mDebugVertexBufferOffset + 1].Color = C;
		mDebugVertexBuffer[mDebugVertexBufferOffset + 2].Color = C;
		mDebugVertexBuffer[mDebugVertexBufferOffset + 3].Color = C;

		mDebugVertexBuffer[mDebugVertexBufferOffset + 4].Color = C;
		mDebugVertexBuffer[mDebugVertexBufferOffset + 5].Color = C;
		mDebugVertexBuffer[mDebugVertexBufferOffset + 6].Color = C;
		mDebugVertexBuffer[mDebugVertexBufferOffset + 7].Color = C;

		mDebugIndexBuffer[mDebugIndexBufferOffset + 0] = mDebugVertexBufferOffset + 0;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 1] = mDebugVertexBufferOffset + 1;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 2] = mDebugVertexBufferOffset + 0;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 3] = mDebugVertexBufferOffset + 2;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 4] = mDebugVertexBufferOffset + 2;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 5] = mDebugVertexBufferOffset + 3;

		mDebugIndexBuffer[mDebugIndexBufferOffset + 6] = mDebugVertexBufferOffset + 3;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 7] = mDebugVertexBufferOffset + 1;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 8] = mDebugVertexBufferOffset + 4;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 9] = mDebugVertexBufferOffset + 5;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 10] = mDebugVertexBufferOffset + 4;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 11] = mDebugVertexBufferOffset + 6;

		mDebugIndexBuffer[mDebugIndexBufferOffset + 12] = mDebugVertexBufferOffset + 6;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 13] = mDebugVertexBufferOffset + 7;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 14] = mDebugVertexBufferOffset + 7;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 15] = mDebugVertexBufferOffset + 5;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 16] = mDebugVertexBufferOffset + 0;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 17] = mDebugVertexBufferOffset + 4;

		mDebugIndexBuffer[mDebugIndexBufferOffset + 18] = mDebugVertexBufferOffset + 1;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 19] = mDebugVertexBufferOffset + 5;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 20] = mDebugVertexBufferOffset + 2;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 21] = mDebugVertexBufferOffset + 6;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 22] = mDebugVertexBufferOffset + 3;
		mDebugIndexBuffer[mDebugIndexBufferOffset + 23] = mDebugVertexBufferOffset + 7;

		mDebugVertexBufferOffset += 8;
		mDebugIndexBufferOffset += 24;
	}
}
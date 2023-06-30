#pragma once

#include <queue>
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
	struct RenderTask
	{
		const Transform* TransformPtr;
		const Mesh<DefaultVertex, U32>* MeshPtr;
		const Texture2D* TexturePtr;
	};

	class DefaultRenderer
	{
	public:

		DefaultRenderer(Scene* Scene);
		virtual ~DefaultRenderer();

	public:

		void Render();

	public:

		void AddRenderTask(const RenderTask& RenderTask);

	private:

		Scene* mScene;
		Shader* mShader;

		std::queue<RenderTask> mRenderQueue = {};
	};
}
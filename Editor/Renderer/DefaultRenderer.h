#pragma once

#include <vector>

#include <Common/Types.h>

namespace ark
{
	class Shader;
	class Transform;
	class Renderable;

	struct RenderTask
	{
		Transform* Transform;
		Renderable* Renderable;
	};

	class DefaultRenderer
	{
	public:

		DefaultRenderer();
		virtual ~DefaultRenderer();

	public:

		void Render();

	public:

		void FlushRenderTasks();
		void AddRenderTask(const RenderTask& RenderTask);

	private:

		Shader* mShader;

		std::vector<RenderTask> mRenderTasks = {};
	};
}

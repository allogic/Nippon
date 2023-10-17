#pragma once

#include <queue>
#include <filesystem>

#include <Common/Types.h>

namespace ark
{
	namespace fs = std::filesystem;

	class Actor;
	class Shader;

	class DefaultRenderer
	{
	public:

		DefaultRenderer();
		virtual ~DefaultRenderer();

	public:

		void Render();

	public:

		void AddToRenderQueue(Actor* Actor);

	private:

		Shader* mShader;

		std::queue<Actor*> mRenderQueue = {};
	};
}

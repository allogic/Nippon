#pragma once

#include <string>
#include <filesystem>
#include <sstream>
#include <vector>

#include <Common/Types.h>

namespace ark
{
	namespace fs = std::filesystem;

	class Scene;
	class Actor;

	class WavefrontExporter
	{
	public:

		static void Export(Actor* Actor, Scene* Scene);

	private:

		static void ExportObjectsRecursive(Actor* Actor, std::ostringstream& Stream, U32& VertexOffset);
		static void ExportMaterialsRecursive(Actor* Actor, std::ostringstream& Stream);
		static void ExportTexturesRecursive(Actor* Actor, Scene* Scene, const fs::path& ExportDir);
	};
}

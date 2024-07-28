#pragma once

#include <Common/Forward.h>
#include <Common/Types.h>
#include <Common/Standard.h>

#include <Editor/Forward.h>

namespace Nippon
{
	class WavefrontExporter
	{
	public:

		static void Export(Entity* Entity, Scene* Scene);

	private:

		static void ExportObjectsRecursive(Entity* Entity, std::ostringstream& Stream, U32& VertexOffset);
		static void ExportMaterialsRecursive(Entity* Entity, std::ostringstream& Stream);
		static void ExportTexturesRecursive(Entity* Entity, const fs::path& ExportDir);
	};
}
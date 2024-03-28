#pragma once

#include <Forward.h>
#include <Types.h>
#include <Standard.h>

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
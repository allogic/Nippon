#pragma once

#include <string>
#include <filesystem>
#include <sstream>
#include <vector>

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
	class WavefrontExporter
	{
	public:

		static void ExportEntity(const std::string& Entry, const std::string& SubEntry);
		static void ExportLevel(const std::string& Entry, const std::string& SubEntry);

	private:

		static void ExportObjectsRecursive(Actor* Node, std::ostringstream& Stream, U32& VertexOffset);
		static void ExportMaterialsRecursive(Actor* Node, std::ostringstream& Stream);
	};
}
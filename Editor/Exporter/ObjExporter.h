#pragma once

#include <string>
#include <filesystem>
#include <sstream>
#include <vector>

#include <Common/Types.h>

#include <Editor/Vertex.h>

#include <Vendor/rapidjson/rapidjson.h>
#include <Vendor/rapidjson/document.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace rj = rapidjson;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
	class ObjExporter
	{
	public:

		static void ExportEntity(const std::string& Entry, const std::string& SubEntry);
		static void ExportLevel(const std::string& Entry, const std::string& SubEntry);

	private:

		static void ExportObject(
			std::ostringstream& Stream,
			const std::string& Entry,
			const std::string& SubEntry,
			Renderable* Renderable,
			U32& VertexOffset);
		static void ExportMaterial(
			std::ostringstream& Stream,
			const std::string& Entry,
			const std::string& SubEntry,
			Renderable* Renderable);
		static void ExportTextures(
			const fs::path& ExportDir,
			const std::string& Entry,
			const std::string& SubEntry,
			const std::vector<Texture2D*>& Textures);
	};
}
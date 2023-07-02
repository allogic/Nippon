#include <Common/Utils/FsUtils.h>
#include <Common/Utils/TextureUtils.h>

#include <Editor/Scenes/EntityScene.h>
#include <Editor/Scenes/LevelScene.h>

#include <Editor/Converter/ElementConverter.h>
#include <Editor/Converter/VertexConverter.h>

#include <Editor/Exporter/ObjExporter.h>

#include <Editor/Components/Renderable.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern rj::Document gConfig;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	void ObjExporter::ExportEntity(const std::string& Entry, const std::string& SubEntry)
	{
		EntityScene scene = { Entry, SubEntry };

		fs::path exportDir = gConfig["exportDir"].GetString();

		FsUtils::CreateIfNotExists(exportDir);
		FsUtils::CreateIfNotExists(exportDir / Entry);
		FsUtils::CreateIfNotExists(exportDir / Entry / SubEntry);

		std::ostringstream objectStream = {};
		std::ostringstream materialStream = {};

		objectStream << "mtllib object.mtl" << "\n";

		U32 vertexOffset = 0;

		for (const auto& actor : scene.GetActors())
		{
			Renderable* renderable = actor->GetComponent<Renderable>();

			if (renderable)
			{
				ExportObject(objectStream, Entry, SubEntry, renderable, vertexOffset);
				ExportMaterial(materialStream, Entry, SubEntry, renderable);
			}
		}

		FsUtils::WriteText(exportDir / Entry / SubEntry / "object.obj", objectStream.str());
		FsUtils::WriteText(exportDir / Entry / SubEntry / "object.mtl", materialStream.str());

		ExportTextures(exportDir, Entry, SubEntry, scene.GetTextures());
	}

	void ObjExporter::ExportLevel(const std::string& Entry, const std::string& SubEntry)
	{
		LevelScene scene = { Entry, SubEntry };
	}

	void ObjExporter::ExportObject(
		std::ostringstream& Stream,
		const std::string& Entry,
		const std::string& SubEntry,
		Renderable* Renderable,
		U32& VertexOffset)
	{
		const auto& vertexBuffer = Renderable->GetVertexBuffer();
		const auto& elementBuffer = Renderable->GetElementBuffer();
		U32 textureIndex = Renderable->GetTextureIndex();

		std::string entityName = Entry + "_" + SubEntry + "_" + std::to_string(textureIndex);
		std::string materialName = Entry + "_" + SubEntry + "_" + std::to_string(textureIndex);
		std::string textureName = "texture_" + std::to_string(textureIndex) + ".png";

		Stream << "o " << entityName << "\n";

		for (const auto& vertex : vertexBuffer)
		{
			Stream << "v " << vertex.Position.x << " " << vertex.Position.y << " " << vertex.Position.z << "\n";
		}

		for (const auto& vertex : vertexBuffer)
		{
			Stream << "vt " << vertex.TextureMap.x << " " << vertex.TextureMap.y << "\n";
		}

		Stream << "usemtl " << materialName << "\n";
		Stream << "s off\n";

		for (U32 i = 0; i < (elementBuffer.size() - 2); i += 3)
		{
			U32 v0 = VertexOffset + elementBuffer[i + 0] + 1;
			U32 v1 = VertexOffset + elementBuffer[i + 2] + 1;
			U32 v2 = VertexOffset + elementBuffer[i + 1] + 1;

			Stream << "f " << v0 << "/" << v0 << " " << v1 << "/" << v1 << " " << v2 << "/" << v2 << "\n";
		}

		Stream << "\n";

		VertexOffset += (U32)vertexBuffer.size();
	}

	void ObjExporter::ExportMaterial(
		std::ostringstream& Stream,
		const std::string& Entry,
		const std::string& SubEntry,
		Renderable* Renderable)
	{
		U32 textureIndex = Renderable->GetTextureIndex();

		std::string entityName = Entry + "_" + SubEntry + "_" + std::to_string(textureIndex);
		std::string materialName = Entry + "_" + SubEntry + "_" + std::to_string(textureIndex);
		std::string textureName = "texture_" + std::to_string(textureIndex) + ".png";

		Stream << "newmtl " << materialName << "\n";
		Stream << "d 1.0\n";
		Stream << "illum 0\n";
		Stream << "map_Kd " << textureName << "\n";
		Stream << "\n";
	}

	void ObjExporter::ExportTextures(
		const fs::path& ExportDir,
		const std::string& Entry,
		const std::string& SubEntry,
		const std::vector<Texture2D*>& Textures)
	{
		for (U32 i = 0; i < Textures.size(); i++)
		{
			std::string textureName = "texture_" + std::to_string(i) + ".png";

			std::vector<U8> bytes = Textures[i]->Snapshot();

			TextureUtils::WritePNG(Textures[i]->GetWidth(), Textures[i]->GetHeight(), bytes, ExportDir / Entry / SubEntry / textureName);
		}
	}
}
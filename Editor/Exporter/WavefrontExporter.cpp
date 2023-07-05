#include <Common/Utils/FsUtils.h>
#include <Common/Utils/TextureUtils.h>

#include <Editor/Editor.h>

#include <Editor/Scenes/EntityScene.h>
#include <Editor/Scenes/LevelScene.h>

#include <Editor/Converter/ElementConverter.h>
#include <Editor/Converter/VertexConverter.h>

#include <Editor/Exporter/WavefrontExporter.h>

#include <Editor/Components/Transform.h>
#include <Editor/Components/Renderable.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	void WavefrontExporter::ExportEntity(const std::string& Entry, const std::string& SubEntry)
	{
		EntityScene scene = { Entry, SubEntry };

		fs::path exportDir = gConfig["exportDir"].GetString();

		FsUtils::CreateIfNotExists(exportDir);
		FsUtils::CreateIfNotExists(exportDir / Entry);
		FsUtils::CreateIfNotExists(exportDir / Entry / SubEntry);

		Actor* staticGeometry = scene.GetStaticGeometryActor();

		std::ostringstream objectStream = {};
		std::ostringstream materialStream = {};

		U32 vertexOffset = 0;

		objectStream << "mtllib object.mtl" << "\n";

		ExportObjectsRecursive(staticGeometry, objectStream, vertexOffset);
		ExportMaterialsRecursive(staticGeometry, materialStream);

		FsUtils::WriteText(exportDir / Entry / SubEntry / "object.obj", objectStream.str());
		FsUtils::WriteText(exportDir / Entry / SubEntry / "object.mtl", materialStream.str());

		const auto& textures = scene.GetMdTextures();

		for (U32 i = 0; i < textures.size(); i++)
		{
			std::string textureName = "texture_" + std::to_string(i) + ".png";

			std::vector<U8> bytes = textures[i]->Snapshot();

			TextureUtils::WritePNG(textures[i]->GetWidth(), textures[i]->GetHeight(), bytes, exportDir / Entry / SubEntry / textureName);
		}
	}

	void WavefrontExporter::ExportLevel(const std::string& Entry, const std::string& SubEntry)
	{
		LevelScene scene = { Entry, SubEntry };

		fs::path exportDir = gConfig["exportDir"].GetString();

		FsUtils::CreateIfNotExists(exportDir);
		FsUtils::CreateIfNotExists(exportDir / Entry);
		FsUtils::CreateIfNotExists(exportDir / Entry / SubEntry);

		Actor* staticGeometry = scene.GetStaticGeometryActor();

		std::ostringstream objectStream = {};
		std::ostringstream materialStream = {};

		U32 vertexOffset = 0;

		objectStream << "mtllib object.mtl" << "\n";

		ExportObjectsRecursive(staticGeometry, objectStream, vertexOffset);
		ExportMaterialsRecursive(staticGeometry, materialStream);

		FsUtils::WriteText(exportDir / Entry / SubEntry / "object.obj", objectStream.str());
		FsUtils::WriteText(exportDir / Entry / SubEntry / "object.mtl", materialStream.str());

		const auto& textures = scene.GetScrTextures();

		for (U32 i = 0; i < textures.size(); i++)
		{
			std::string textureName = "texture_" + std::to_string(i) + ".png";

			std::vector<U8> bytes = textures[i]->Snapshot();

			TextureUtils::WritePNG(textures[i]->GetWidth(), textures[i]->GetHeight(), bytes, exportDir / Entry / SubEntry / textureName);
		}
	}

	void WavefrontExporter::ExportObjectsRecursive(Actor* Node, std::ostringstream& Stream, U32& VertexOffset)
	{
		Renderable* renderable = Node->GetComponent<Renderable>();

		if (renderable)
		{
			U32 textureIndex = renderable->GetTextureIndex();

			std::string objectName = Node->GetName() + "_" + std::to_string(textureIndex);
			std::string materialName = Node->GetName() + "_" + std::to_string(textureIndex);

			Transform* transform = Node->GetTransform();
			Renderable* renderable = Node->GetComponent<Renderable>();

			const auto& vertexBuffer = renderable->GetVertexBuffer();
			const auto& elementBuffer = renderable->GetElementBuffer();

			Stream << "o " << objectName << "\n";

			for (const auto& vertex : vertexBuffer)
			{
				R32V3 position = transform->GetWorldPosition() + vertex.Position;

				Stream << "v " << position.x << " " << position.y << " " << position.z << "\n";
			}

			for (const auto& vertex : vertexBuffer)
			{
				R32V2 uv = vertex.TextureMap;

				Stream << "vt " << uv.x << " " << uv.y << "\n";
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

		for (Actor* child : Node->GetChildren())
		{
			ExportObjectsRecursive(child, Stream, VertexOffset);
		}
	}

	void WavefrontExporter::ExportMaterialsRecursive(Actor* Node, std::ostringstream& Stream)
	{
		Renderable* renderable = Node->GetComponent<Renderable>();

		if (renderable)
		{
			U32 textureIndex = renderable->GetTextureIndex();

			std::string materialName = Node->GetName() + "_" + std::to_string(textureIndex);

			Stream << "newmtl " << materialName << "\n";
			Stream << "d 1.0\n";
			Stream << "illum 0\n";
			Stream << "map_Kd " << "texture_" << textureIndex << ".png\n";

			Stream << "\n";
		}

		for (Actor* child : Node->GetChildren())
		{
			ExportMaterialsRecursive(child, Stream);
		}
	}
}
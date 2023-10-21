#include <Common/Macros.h>

#include <Common/Utilities/FsUtils.h>
#include <Common/Utilities/TextureUtils.h>

#include <Editor/Editor.h>
#include <Editor/Scene.h>
#include <Editor/Actor.h>
#include <Editor/Magic.h>

#include <Editor/Scenes/LevelScene.h>
#include <Editor/Scenes/EntityScene.h>

#include <Editor/Converter/ElementConverter.h>
#include <Editor/Converter/VertexConverter.h>

#include <Editor/Exporter/WavefrontExporter.h>

#include <Editor/Components/Transform.h>
#include <Editor/Components/Renderable.h>

#include <Editor/Glad/glad.h>

namespace ark
{
	void WavefrontExporter::Export(Actor* Actor, Scene* Scene)
	{
		fs::path exportDir = "Exports";

		FsUtils::CreateDirIfNotExist(exportDir / Scene->GetDatArchiveFileName() / Actor->GetName());

		std::ostringstream objectStream = {};
		std::ostringstream materialStream = {};

		U32 vertexOffset = 0;

		objectStream << "mtllib object.mtl" << "\n";

		LOG("\n");
		LOG(" Exporting Actor %s\n", Actor->GetName().c_str());
		LOG("-----------------------------------------------------------------------------------------\n");
		LOG("\n");
		LOG("Creating objects:\n");

		ExportObjectsRecursive(Actor, objectStream, vertexOffset);

		LOG("\n");
		LOG("Creating materials:\n");

		ExportMaterialsRecursive(Actor, materialStream);

		LOG("\n");
		LOG("Creating textures:\n");

		ExportTexturesRecursive(Actor, exportDir / Scene->GetDatArchiveFileName() / Actor->GetName());

		LOG("\n");

		FsUtils::WriteText(exportDir / Scene->GetDatArchiveFileName() / Actor->GetName() / "object.obj", objectStream.str());
		FsUtils::WriteText(exportDir / Scene->GetDatArchiveFileName() / Actor->GetName() / "object.mtl", materialStream.str());
	}

	void WavefrontExporter::ExportObjectsRecursive(Actor* Actor, std::ostringstream& Stream, U32& VertexOffset)
	{
		if (Renderable* renderable = Actor->GetComponent<Renderable>())
		{
			U32 textureIndex = renderable->GetTextureIndex();

			if (textureIndex < 0xFF)
			{
				std::string objectName = Actor->GetName() + "_" + std::to_string(textureIndex);
				std::string materialName = Actor->GetName() + "_" + std::to_string(textureIndex);

				LOG("    %s\n", objectName.c_str());

				Transform* transform = Actor->GetTransform();
				Renderable* renderable = Actor->GetComponent<Renderable>();

				const auto& vertexBuffer = renderable->GetVertexBuffer();
				const auto& elementBuffer = renderable->GetElementBuffer();

				Stream << "o " << objectName << "\n";

				for (const auto& vertex : vertexBuffer)
				{
					R32V3 position = (transform->GetWorldPosition() / MAGIC_WORLD_SCALE) + transform->GetWorldRotation() * (vertex.Position * (transform->GetWorldScale() / MAGIC_WORLD_SCALE));

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
		}

		for (const auto& child : Actor->GetChildren())
		{
			ExportObjectsRecursive(child, Stream, VertexOffset);
		}
	}

	void WavefrontExporter::ExportMaterialsRecursive(Actor* Actor, std::ostringstream& Stream)
	{
		if (Renderable* renderable = Actor->GetComponent<Renderable>())
		{
			U32 textureIndex = renderable->GetTextureIndex();

			if (textureIndex < 0xFF)
			{
				std::string materialName = Actor->GetName() + "_" + std::to_string(textureIndex);

				LOG("    %s\n", materialName.c_str());

				Stream << "newmtl " << materialName << "\n";
				Stream << "d 1.0\n";
				Stream << "illum 0\n";
				Stream << "map_Kd " << "texture_" << textureIndex << ".png\n";

				Stream << "\n";
			}
		}

		for (const auto& child : Actor->GetChildren())
		{
			ExportMaterialsRecursive(child, Stream);
		}
	}

	void WavefrontExporter::ExportTexturesRecursive(Actor* Actor, const fs::path& ExportDir)
	{
		if (Renderable* renderable = Actor->GetComponent<Renderable>())
		{
			U32 textureIndex = renderable->GetTextureIndex();

			if (textureIndex < 0xFF)
			{
				std::string textureName = "texture_" + std::to_string(textureIndex) + ".png";

				if (!fs::exists(ExportDir / textureName))
				{
					LOG("    %s\n", textureName.c_str());

					if (Texture2D* texture = renderable->GetTexture())
					{
						std::vector<U8> bytes = texture->Snapshot(4, GL_UNSIGNED_BYTE);

						TextureUtils::WritePNG(texture->GetWidth(), texture->GetHeight(), bytes, ExportDir / textureName);
					}
				}
			}
		}

		for (const auto& child : Actor->GetChildren())
		{
			ExportTexturesRecursive(child, ExportDir);
		}
	}
}

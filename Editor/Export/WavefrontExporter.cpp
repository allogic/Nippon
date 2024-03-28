#include <Magic.h>

#include <Ecs/Entity.h>
#include <Ecs/Registry.h>

#include <Ecs/Components/Transform.h>
#include <Ecs/Components/Renderable.h>

#include <Export/WavefrontExporter.h>

#include <Interface/Log.h>

#include <OpenGl/StaticMesh.h>
#include <OpenGl/VertexTypes.h>

#include <Scene/Scene.h>

#include <Utility/FileUtility.h>
#include <Utility/TextureUtility.h>

namespace Nippon
{
	void WavefrontExporter::Export(Entity* Entity, Scene* Scene)
	{
		fs::path exportDir = "Exported";

		FileUtility::CreateDirIfNotExist(exportDir / Scene->GetFolderId() / Scene->GetArchiveId() / Entity->GetName());

		std::ostringstream objectStream = {};
		std::ostringstream materialStream = {};

		U32 vertexOffset = 0;

		objectStream << "mtllib object.mtl" << "\n";

		Log::Add("Exporting Actor %s\n", Entity->GetName().data());
		Log::Add("  Creating objects:\n");

		ExportObjectsRecursive(Entity, objectStream, vertexOffset);

		Log::Add("\n");
		Log::Add("  Creating materials:\n");

		ExportMaterialsRecursive(Entity, materialStream);

		Log::Add("\n");
		Log::Add("  Creating textures:\n");

		ExportTexturesRecursive(Entity, exportDir / Scene->GetFolderId() / Scene->GetArchiveId() / Entity->GetName());

		FileUtility::WriteText(exportDir / Scene->GetFolderId() / Scene->GetArchiveId() / Entity->GetName() / "object.obj", objectStream.str());
		FileUtility::WriteText(exportDir / Scene->GetFolderId() / Scene->GetArchiveId() / Entity->GetName() / "object.mtl", materialStream.str());
	}

	void WavefrontExporter::ExportObjectsRecursive(Entity* Entity, std::ostringstream& Stream, U32& VertexOffset)
	{
		if (Renderable* renderable = Entity->GetComponent<Renderable>())
		{
			U32 textureIndex = renderable->GetTextureIndex();

			if (textureIndex < 0xFF)
			{
				std::string objectName = Entity->GetName() + "_" + std::to_string(textureIndex);
				std::string materialName = Entity->GetName() + "_" + std::to_string(textureIndex);

				Log::Add("    %s\n", objectName.data());

				Transform* transform = Entity->GetTransform();
				Renderable* renderable = Entity->GetComponent<Renderable>();

				StaticMesh* staticMesh = renderable->GetStaticMesh();

				U64 vertexBufferCount = staticMesh->GetVertexBufferCount();
				U64 indexBufferCount = staticMesh->GetIndexBufferCount();

				std::vector<DefaultVertex> vertices = {};
				std::vector<U32> indices = {};

				vertices.resize(vertexBufferCount);
				indices.resize(indexBufferCount);

				staticMesh->GetVertices(vertices.data(), sizeof(DefaultVertex), vertexBufferCount);
				staticMesh->GetIndices(indices.data(), sizeof(U32), indexBufferCount);

				Stream << "o " << objectName << "\n";

				for (const auto& vertex : vertices)
				{
					R32V3 position = (transform->GetWorldPosition() / MAGIC_WORLD_SCALE) + transform->GetWorldRotation() * (vertex.Position * (transform->GetWorldScale() / MAGIC_WORLD_SCALE));

					Stream << "v " << position.x << " " << position.y << " " << position.z << "\n";
				}

				for (const auto& vertex : vertices)
				{
					R32V2 uv = vertex.TextureMap;

					Stream << "vt " << uv.x << " " << uv.y << "\n";
				}

				Stream << "usemtl " << materialName << "\n";
				Stream << "s off\n";

				for (U32 i = 0; i < (indexBufferCount - 2); i += 3)
				{
					U32 v0 = VertexOffset + indices[i + 0] + 1;
					U32 v1 = VertexOffset + indices[i + 2] + 1;
					U32 v2 = VertexOffset + indices[i + 1] + 1;

					Stream << "f " << v0 << "/" << v0 << " " << v1 << "/" << v1 << " " << v2 << "/" << v2 << "\n";
				}

				Stream << "\n";

				VertexOffset += (U32)vertexBufferCount;
			}
		}

		for (const auto& child : Entity->GetChildren())
		{
			ExportObjectsRecursive(child, Stream, VertexOffset);
		}
	}

	void WavefrontExporter::ExportMaterialsRecursive(Entity* Entity, std::ostringstream& Stream)
	{
		if (Renderable* renderable = Entity->GetComponent<Renderable>())
		{
			U32 textureIndex = renderable->GetTextureIndex();

			if (textureIndex < 0xFF)
			{
				std::string materialName = Entity->GetName() + "_" + std::to_string(textureIndex);

				Log::Add("    %s\n", materialName.data());

				Stream << "newmtl " << materialName << "\n";
				Stream << "d 1.0\n";
				Stream << "illum 0\n";
				Stream << "map_Kd " << "texture_" << textureIndex << ".png\n";

				Stream << "\n";
			}
		}

		for (const auto& child : Entity->GetChildren())
		{
			ExportMaterialsRecursive(child, Stream);
		}
	}

	void WavefrontExporter::ExportTexturesRecursive(Entity* Entity, const fs::path& ExportDir)
	{
		if (Renderable* renderable = Entity->GetComponent<Renderable>())
		{
			U32 textureIndex = renderable->GetTextureIndex();

			if (textureIndex < 0xFF)
			{
				std::string textureName = "texture_" + std::to_string(textureIndex) + ".png";

				if (!fs::exists(ExportDir / textureName))
				{
					Log::Add("    %s\n", textureName.data());

					if (U32 texture = renderable->GetTexture())
					{
						TextureUtility::WritePNG(texture, ExportDir / textureName);
					}
				}
			}
		}

		for (const auto& child : Entity->GetChildren())
		{
			ExportTexturesRecursive(child, ExportDir);
		}
	}
}
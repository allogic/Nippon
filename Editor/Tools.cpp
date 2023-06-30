#include <Common/Debug.h>

#include <Common/Utils/StringUtils.h>
#include <Common/Utils/TextureUtils.h>

#include <Editor/Tools.h>

#include <Editor/Components/Transform.h>

#include <Editor/Scenes/EntityScene.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern rj::Document gConfig;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
	void Tools::GenerateEntityThumbnail(const std::string& Entry, const std::string& SubEntry, rj::Value& Value)
	{
		EntityScene scene = { Entry, SubEntry };

		Actor* player = scene.GetMainActor();

		rj::Value& thumbnail = Value["thumbnail"];

		R32 distance = thumbnail["distance"].GetFloat();
		R32 height = thumbnail["height"].GetFloat();
		R32 pitch = thumbnail["pitch"].GetFloat();

		player->GetTransform()->SetLocalPosition(R32V3{ distance, height, distance });
		player->GetTransform()->SetLocalRotation(R32V3{ -45.0F, pitch, 0.0F });

		scene.Resize(256, 256);
		scene.Update();
		scene.Render();

		std::vector<U8> bytes = scene.Snapshot();

		fs::path file = fs::path{ gConfig["unpackDir"].GetString() } / Entry / SubEntry / "thumb.png";

		TextureUtils::WritePNG(scene.GetWidth(), scene.GetHeight(), bytes, file);

		std::string posixFile = StringUtils::PosixPath(file.string());

		LOG("%s\n", posixFile.c_str());
	}
}
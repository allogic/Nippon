#include <Common/Debug.h>

#include <Common/Utils/FsUtils.h>
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

		R32 x = thumbnail["x"].GetFloat();
		R32 y = thumbnail["y"].GetFloat();
		R32 z = thumbnail["z"].GetFloat();
		R32 pitch = thumbnail["pitch"].GetFloat();

		player->GetTransform()->SetLocalPosition(R32V3{ x, y, z });
		player->GetTransform()->SetLocalRotation(R32V3{ pitch, 45.0F, 0.0F });

		scene.Resize(256, 256);
		scene.Update();
		scene.Render();

		std::vector<U8> bytes = scene.Snapshot();

		fs::path thumbnailDir = gConfig["thumbnailDir"].GetString();
		fs::path thumbnailFile = thumbnailDir / Entry / SubEntry / "thumb.png";

		FsUtils::CreateIfNotExists(thumbnailDir);
		FsUtils::CreateIfNotExists(thumbnailDir / Entry);
		FsUtils::CreateIfNotExists(thumbnailDir / Entry / SubEntry);

		TextureUtils::WritePNG(scene.GetWidth(), scene.GetHeight(), bytes, thumbnailFile);

		std::string posixFile = StringUtils::PosixPath(thumbnailFile.string());

		LOG("%s\n", posixFile.c_str());
	}
}
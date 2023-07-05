#pragma once

#include <string>
#include <vector>
#include <set> // TODO: Remove me

#include <Common/Types.h>

#include <Editor/Forward.h>
#include <Editor/Header.h>
#include <Editor/Model.h>
#include <Editor/Scene.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
	class LevelScene : public Scene
	{
	public:

		LevelScene(
			const std::string& Entry,
			const std::string& SubEntry);
		LevelScene(
			const std::string& Entry,
			const std::string& SubEntry,
			const std::string& SceneName,
			const std::string& WindowName);
		virtual ~LevelScene();

	public:

		inline const auto& GetScrTextures() const { return mScrTextures; }

	protected:

		virtual void Load() override;
		virtual void Save() override;

	private:

		void LoadMd(const std::vector<ObjEntry>& Objects);

	private:

		void AddStaticGeometry(const ScrGroup& Group);

	private:

		std::vector<Texture2D*> mScrTextures = {};
	};
}
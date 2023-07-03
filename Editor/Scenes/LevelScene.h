#pragma once

#include <string>
#include <vector>
#include <utility>

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

		inline const auto& GetModels() const { return mModels; }
		inline const auto& GetObjects() const { return mObjects; }

	protected:

		virtual void Load() override;
		virtual void Save() override;

	private:

		void ModelsToActors();
		void ObjectsToActors();

	private:

		std::vector<std::pair<ScrModel, ScrTransform>> mModels = {};
		std::vector<ObjEntry> mObjects = {};
	};
}
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
	class EntityScene : public Scene
	{
	public:

		EntityScene(
			const std::string& Entry,
			const std::string& SubEntry);
		EntityScene(
			const std::string& Entry,
			const std::string& SubEntry,
			const std::string& SceneName,
			const std::string& WindowName);
		virtual ~EntityScene();

	public:

		inline const auto& GetModels() const { return mModels; }

	protected:

		virtual void Load() override;
		virtual void Save() override;

	private:

		void ModelsToActors();

	private:

		std::vector<std::pair<MdModel, MdTransform>> mModels = {};
	};
}
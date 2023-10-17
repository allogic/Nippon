#pragma once

#include <vector>
#include <filesystem>

#include <Common/Types.h>

#include <Editor/Interface.h>

namespace ark
{
	namespace fs = std::filesystem;

	class MainMenu : public Interface
	{
	public:

		MainMenu();
		virtual ~MainMenu();

	public:

		virtual void Reset() override;
		virtual void Render() override;

	private:

		void RenderFileMenu();
		void RenderEditMenu();
		void RenderLevelMenu();
		void RenderEntityMenu();
		void RenderArchiveMenu();
		void RenderToolsMenu();
	};
}

#pragma once

#include <Common/Types.h>

#include <Editor/Interface.h>

namespace ark
{
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
		void RenderToolsMenu();
	};
}

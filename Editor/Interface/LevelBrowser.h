#pragma once

#include <Common/Types.h>

#include <Editor/Interface.h>

namespace ark
{
	class LevelBrowser : public Interface
	{
	public:

		LevelBrowser();
		virtual ~LevelBrowser();

	public:

		virtual void Reset() override;
		virtual void Render() override;
	};
}

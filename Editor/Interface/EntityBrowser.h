#pragma once

#include <Common/Types.h>

#include <Editor/Interface.h>

namespace ark
{
	class EntityBrowser : public Interface
	{
	public:

		EntityBrowser();
		virtual ~EntityBrowser();

	public:

		virtual void Reset() override;
		virtual void Render() override;
	};
}

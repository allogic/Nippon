#pragma once

#include <limits>

#include <Common/Types.h>

#include <Editor/Interface.h>

namespace ark
{
	class Inspector : public Interface
	{
	public:

		virtual void Reset() override;
		virtual void Render() override;
	};
}

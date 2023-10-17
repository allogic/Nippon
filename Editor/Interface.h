#pragma once

namespace ark
{
	class Interface
	{
	public:

		virtual ~Interface() {};

	public:

		virtual void Reset() = 0;
		virtual void Render() = 0;
	};
}

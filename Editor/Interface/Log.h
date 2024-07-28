#pragma once

#include <Common/Standard.h>
#include <Common/Types.h>

#include <Editor/Forward.h>

namespace Nippon
{
	class Log
	{
	public:

		static void Reset();
		static void Render();

	public:

		template<typename ... Args>
		static void Add(std::string const& Format, Args&& ... Arguments);

	private:

		static inline std::mutex sMutex = {};

		static inline std::string sBuffer = "";

		static inline U32 sMaxLineCount = 1024;
		static inline U32 sLineCount = 0;
	};
}

namespace Nippon
{
	template<typename ... Args>
	void Log::Add(std::string const& Format, Args&& ... Arguments)
	{
		static char buffer[1024] = { 0 };

		std::lock_guard<std::mutex> lock{ sMutex };

		std::snprintf(buffer, sizeof(buffer), Format.data(), std::forward<Args>(Arguments) ...);

		sBuffer.append(buffer);

		char* character = buffer;

		bool newLineFound = false;

		while (*character != 0)
		{
			if (*character == '\n')
			{
				newLineFound = true;

				sLineCount++;
			}

			character++;
		}

		if (newLineFound)
		{
			if (sLineCount >= sMaxLineCount)
			{
				U64 firstNewLine = sBuffer.find_first_of('\n') + 1;

				sBuffer.erase(0, firstNewLine);
			}
		}
	}
}
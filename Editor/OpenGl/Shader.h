#pragma once

#include <Common/Standard.h>
#include <Common/Types.h>

#include <Editor/Forward.h>

namespace Nippon
{
	class Shader
	{
	public:

		Shader(fs::path const& VertexFile, fs::path const& FragmentFile);
		virtual ~Shader();

	public:

		void Bind();
		void UnBind();

	public:

		static bool CheckCompileStatus(U32 Shader);
		static bool CheckLinkStatus(U32 Program);

	public:

		void SetUniformR32(std::string const& Name, R32 Value);

		void SetUniformR32V2(std::string const& Name, R32V2 const& Value);
		void SetUniformR32V3(std::string const& Name, R32V3 const& Value);
		void SetUniformR32M3(std::string const& Name, R32M3 const& Value);
		void SetUniformR32M4(std::string const& Name, R32M4 const& Value);

	private:

		U32 mProgram = 0;
	};
}
#include <Glad/glad.h>

#include <Interface/Log.h>

#include <OpenGl/Shader.h>

#include <Utility/FileUtility.h>

namespace Nippon
{
	Shader::Shader(fs::path const& VertexFile, fs::path const& FragmentFile)
	{
		std::string vertexShaderSrc = FileUtility::ReadText(VertexFile);
		std::string fragmentShaderSrc = FileUtility::ReadText(FragmentFile);

		mProgram = glCreateProgram();

		U32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
		U32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		char const* vertexShaderSrcPtr = vertexShaderSrc.data();
		char const* fragmentShaderSrcPtr = fragmentShaderSrc.data();

		glShaderSource(vertexShader, 1, &vertexShaderSrcPtr, nullptr);
		glCompileShader(vertexShader);
		bool vertexCompileStatus = CheckCompileStatus(vertexShader);

		glShaderSource(fragmentShader, 1, &fragmentShaderSrcPtr, nullptr);
		glCompileShader(fragmentShader);
		bool fragmentCompileStatus = CheckCompileStatus(fragmentShader);

		bool shaderLinkStatus = false;

		if (vertexCompileStatus && fragmentCompileStatus)
		{
			glAttachShader(mProgram, vertexShader);
			glAttachShader(mProgram, fragmentShader);

			glLinkProgram(mProgram);

			shaderLinkStatus = CheckLinkStatus(mProgram);
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		if (!shaderLinkStatus)
		{
			glDeleteProgram(mProgram);

			mProgram = 0;
		}
	}

	Shader::~Shader()
	{
		glDeleteProgram(mProgram);

		mProgram = 0;
	}

	void Shader::Bind()
	{
		glUseProgram(mProgram);
	}

	void Shader::UnBind()
	{
		glUseProgram(0);
	}

	bool Shader::CheckCompileStatus(U32 Shader)
	{
		I32 compileStatus = 0;
		I32 infoLogLength = 0;

		std::string log = "";

		glGetShaderiv(Shader, GL_COMPILE_STATUS, &compileStatus);
		if (compileStatus == 0)
		{
			glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &infoLogLength);
			if (infoLogLength > 0)
			{
				log.resize(infoLogLength);

				glGetShaderInfoLog(Shader, infoLogLength, &infoLogLength, &log[0]);

				Log::Add("%s\n", &log[0]);

				return false;
			}
		}

		return true;
	}

	bool Shader::CheckLinkStatus(U32 Program)
	{
		I32 compileStatus = 0;
		I32 infoLogLength = 0;

		std::string log = "";

		glGetProgramiv(Program, GL_LINK_STATUS, &compileStatus);
		if (compileStatus == 0)
		{
			glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &infoLogLength);
			if (infoLogLength > 0)
			{
				log.resize(infoLogLength);

				glGetProgramInfoLog(Program, infoLogLength, &infoLogLength, &log[0]);

				Log::Add("%s\n", &log[0]);

				return false;
			}
		}

		return true;
	}

	void Shader::SetUniformR32(std::string const& Name, R32 Value)
	{
		glUniform1f(
			glGetUniformLocation(mProgram, Name.data()),
			Value
		);
	}

	void Shader::SetUniformR32V2(std::string const& Name, R32V2 const& Value)
	{
		glUniform2fv(
			glGetUniformLocation(mProgram, Name.data()),
			1,
			&Value[0]
		);
	}

	void Shader::SetUniformR32V3(std::string const& Name, R32V3 const& Value)
	{
		glUniform3fv(
			glGetUniformLocation(mProgram, Name.data()),
			1,
			&Value[0]
		);
	}

	void Shader::SetUniformR32M3(std::string const& Name, R32M3 const& Value)
	{
		glUniformMatrix3fv(
			glGetUniformLocation(mProgram, Name.data()),
			1,
			0,
			&Value[0][0]
		);
	}

	void Shader::SetUniformR32M4(std::string const& Name, R32M4 const& Value)
	{
		glUniformMatrix4fv(
			glGetUniformLocation(mProgram, Name.data()),
			1,
			0,
			&Value[0][0]
		);
	}
}
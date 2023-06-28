#include <Common/Debug.h>

#include <Common/Utils/FsUtils.h>

#include <Editor/Shader.h>

#include <Vendor/GLAD/glad.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  Shader::Shader(const fs::path& VertexFile, const fs::path& FragmentFile)
  {
    std::string vertexShader = FsUtils::ReadText(VertexFile);
    std::string fragmentShader = FsUtils::ReadText(FragmentFile);

    mProgram = glCreateProgram();

    U32 vid = glCreateShader(GL_VERTEX_SHADER);
    U32 fid = glCreateShader(GL_FRAGMENT_SHADER);

    char const* vertexShaderPtr = &vertexShader[0];
    glShaderSource(vid, 1, &vertexShaderPtr, nullptr);
    glCompileShader(vid);
    U32 vsCompileStatus = CheckCompileStatus(vid);

    char const* fragmentShaderPtr = &fragmentShader[0];
    glShaderSource(fid, 1, &fragmentShaderPtr, nullptr);
    glCompileShader(fid);
    U32 fsCompileStatus = CheckCompileStatus(fid);

    U32 shaderLinkStatus = 0;
    if (vsCompileStatus && fsCompileStatus)
    {
      glAttachShader(mProgram, vid);
      glAttachShader(mProgram, fid);
      glLinkProgram(mProgram);
      shaderLinkStatus = CheckLinkStatus(mProgram);
    }

    glDeleteShader(vid);
    glDeleteShader(fid);

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

  void Shader::Bind() const
  {
    glUseProgram(mProgram);
  }

  void Shader::Unbind() const
  {
    glUseProgram(0);
  }

  U32 Shader::CheckCompileStatus(U32 Sid)
  {
    I32 compileStatus = 0;
    I32 infoLogLength = 0;

    std::string log = "";

    glGetShaderiv(Sid, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == 0)
    {
      glGetShaderiv(Sid, GL_INFO_LOG_LENGTH, &infoLogLength);
      if (infoLogLength > 0)
      {
        log.resize(infoLogLength);

        glGetShaderInfoLog(Sid, infoLogLength, &infoLogLength, &log[0]);

        LOG("%s\n", &log[0]);

        return 0;
      }
    }

    return 1;
  }

  U32 Shader::CheckLinkStatus(U32 Pid)
  {
    I32 compileStatus = 0;
    I32 infoLogLength = 0;

    std::string log = "";

    glGetProgramiv(Pid, GL_LINK_STATUS, &compileStatus);
    if (compileStatus == 0)
    {
      glGetProgramiv(Pid, GL_INFO_LOG_LENGTH, &infoLogLength);
      if (infoLogLength > 0)
      {
        log.resize(infoLogLength);

        glGetProgramInfoLog(Pid, infoLogLength, &infoLogLength, &log[0]);

        LOG("%s\n", &log[0]);

        return 0;
      }
    }

    return 1;
  }

  void Shader::SetUniformR32(const std::string& Name, R32 Value) const
  {
    glUniform1f(
      glGetUniformLocation(mProgram, Name.c_str()),
      Value
    );
  }

  void Shader::SetUniformR32V2(const std::string& Name, const R32V2& Value) const
  {
    glUniform2fv(
      glGetUniformLocation(mProgram, Name.c_str()),
      1,
      &Value[0]
    );
  }

  void Shader::SetUniformR32V3(const std::string& Name, const R32V3& Value) const
  {
    glUniform3fv(
      glGetUniformLocation(mProgram, Name.c_str()),
      1,
      &Value[0]
    );
  }

  void Shader::SetUniformR32M3(const std::string& Name, const R32M3& Value) const
  {
    glUniformMatrix3fv(
      glGetUniformLocation(mProgram, Name.c_str()),
      1,
      0,
      &Value[0][0]
    );
  }

  void Shader::SetUniformR32M4(const std::string& Name, const R32M4& Value) const
  {
    glUniformMatrix4fv(
      glGetUniformLocation(mProgram, Name.c_str()),
      1,
      0,
      &Value[0][0]
    );
  }
}
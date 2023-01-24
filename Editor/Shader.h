#pragma once

#include <string>
#include <filesystem>

#include <Common/Types.h>

///////////////////////////////////////////////////////////
// Namespaces
///////////////////////////////////////////////////////////

namespace fs = std::filesystem;

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  class Shader
  {
  public:

    Shader(const fs::path& File);
    virtual ~Shader();

  public:

    void Bind() const;
    void UnBind() const;

  public:

    static void ExtractShaderStages(const fs::path& ShaderFile, std::string& VertexShader, std::string& FragmentShader);
    static U32 CheckCompileStatus(U32 Sid);
    static U32 CheckLinkStatus(U32 Pid);

  public:

    void SetUniformR32(const std::string& Name, R32 Value) const;
    void SetUniformR32M4(const std::string& Name, R32M4 Value) const;

  private:

    U32 mProgram{};
  };
}
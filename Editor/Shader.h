#pragma once

#include <cstring>
#include <regex>
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

    Shader(const fs::path& VertexFile, const fs::path& FragmentFile);
    virtual ~Shader();

  public:

    void Bind() const;
    void Unbind() const;

  public:

    static U32 CheckCompileStatus(U32 Sid);
    static U32 CheckLinkStatus(U32 Pid);

  public:

    void SetUniformR32(const std::string& Name, R32 Value) const;
    void SetUniformR32V2(const std::string& Name, const R32V2& Value) const;
    void SetUniformR32V3(const std::string& Name, const R32V3& Value) const;
    void SetUniformR32M3(const std::string& Name, const R32M3& Value) const;
    void SetUniformR32M4(const std::string& Name, const R32M4& Value) const;

  private:

    U32 mProgram = 0;
  };
}
#include <Common/Utils/ArchiveUtils.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  void ArchiveUtils::FromArchiveName(const std::string& String, U16& Index, std::string& Name, std::string& Type)
  {
    U64 indexOffset = String.find('@');
    U64 nameOffset = String.find('@', indexOffset + 1);

    Index = (U16)std::strtoul(String.substr(0, indexOffset).c_str(), nullptr, 10);
    Name = String.substr(indexOffset + 1, nameOffset - indexOffset - 1);
    Type = String.substr(nameOffset + 1);
  }

  std::string ArchiveUtils::ToArchiveName(U16 Index, const std::string& Name, const std::string& Type)
  {
    std::string result = {};

    result.resize(5 + 2 + Name.size() + Type.size());

    std::snprintf(&result[0], 6, "%05u", Index);
    result[5] = '@';
    std::memcpy(&result[6], &Name[0], Name.size());
    result[6 + Name.size()] = '@';
    std::memcpy(&result[7 + Name.size()], &Type[0], Type.size());

    return result;
  }
}
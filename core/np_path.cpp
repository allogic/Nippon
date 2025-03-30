#include <np_pch.hpp>
#include <np_path.hpp>

NpPath::NpPath() : std::filesystem::path() {}
NpPath::NpPath(NpPath const &Other) : std::filesystem::path(Other) {}
NpPath::NpPath(char const *Other) : std::filesystem::path(Other) {}
NpPath::NpPath(std::string const &Other) : std::filesystem::path(Other) {}
NpPath::NpPath(std::filesystem::path const &Other) : std::filesystem::path(Other) {}

NpPath &NpPath::operator=(NpPath const &Other) {
  std::filesystem::path::operator=(Other);
  return *this;
}
NpPath &NpPath::operator=(char const *Other) {
  std::filesystem::path::operator=(Other);
  return *this;
}
NpPath &NpPath::operator=(std::string const &Other) {
  std::filesystem::path::operator=(Other);
  return *this;
}
NpPath &NpPath::operator=(std::filesystem::path const &Other) {
  std::filesystem::path::operator=(Other);
  return *this;
}

void CreateDir(NpPath const &Path) {
  NpPath PartialPath = "";

  for (auto const &PathSection : Path) {
    PartialPath /= PathSection;

    if (!std::filesystem::exists(PartialPath)) {
      std::filesystem::create_directory(PartialPath);
    }
  }
}

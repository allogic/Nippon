#include <np_core_pch.hpp>
#include <np_core_path_util.hpp>

void NpPathUtil::CreateDir(std::filesystem::path const &File, bool DotIsDirectory) {
  std::filesystem::path PartialPath = "";
  for (auto const &PathSection : File) {
    PartialPath /= PathSection;
    if (!DotIsDirectory) {
      if (File.string().find('.') != File.string().size()) {
        break;
      }
    }
    if (!std::filesystem::exists(PartialPath)) {
      std::filesystem::create_directory(PartialPath);
    }
  }
}

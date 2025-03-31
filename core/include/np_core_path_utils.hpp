#if !defined(NP_CORE_PATH_UTILS_HPP)
#  define NP_CORE_PATH_UTILS_HPP

class NpPathUtils {
public:
  static void CreateDir(std::filesystem::path const &File, bool DotIsDirectory = false);
};

#endif

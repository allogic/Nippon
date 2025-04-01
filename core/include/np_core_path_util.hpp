#if !defined(NP_CORE_PATH_UTIL_HPP)
#  define NP_CORE_PATH_UTIL_HPP

class NpPathUtil {
public:
  static void CreateDir(std::filesystem::path const &File, bool DotIsDirectory = false);
};

#endif

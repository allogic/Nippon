#if !defined(NP_CORE_PATH_HPP)
#  define NP_CORE_PATH_HPP

class NpPath : public std::filesystem::path {
public:
  NpPath();
  NpPath(NpPath const &Other);
  NpPath(char const *Other);
  NpPath(std::string const &Other);
  NpPath(std::filesystem::path const &Other);

public:
  NpPath &operator=(NpPath const &Other);
  NpPath &operator=(char const *Other);
  NpPath &operator=(std::string const &Other);
  NpPath &operator=(std::filesystem::path const &Other);

public:
  void CreateDir(NpPath const &Path);
};

#endif

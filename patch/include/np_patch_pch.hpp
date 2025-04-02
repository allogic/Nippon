#if !defined(NP_PATCH_PCH_HPP)
#  define NP_PATCH_PCH_HPP

#  include <array>
#  include <cassert>
#  include <chrono>
#  include <cstdio>
#  include <cstdlib>
#  include <cstring>
#  include <filesystem>
#  include <fstream>
#  include <functional>
#  include <limits>
#  include <map>
#  include <mutex>
#  include <random>
#  include <set>
#  include <sstream>
#  include <string>
#  include <thread>
#  include <utility>
#  include <vector>

#  if defined(OS_WINDOWS)
#    define WIN32_LEAN_AND_MEAN
#    define NOMINMAX
#    include <windows.h>
#  endif

#endif

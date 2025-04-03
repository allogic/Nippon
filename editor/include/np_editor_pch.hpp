#if !defined(NP_EDITOR_PCH_HPP)
#  define NP_EDITOR_PCH_HPP

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

#  if defined(OS_WINDOWS)
#    define VK_USE_PLATFORM_WIN32_KHR
#    include <vulkan/vulkan.h>
#  elif defined(OS_LINUX)
#    define VK_USE_PLATFORM_WAYLAND_KHR
#    include <vulkan/vulkan.h>
#  endif

#  include <GLFW/glfw3.h>

#  include <glm/glm.hpp>
#  include <glm/gtc/quaternion.hpp>

#  include <backends/imgui_impl_glfw.h>
#  include <backends/imgui_impl_vulkan.h>
#  include <imgui.h>

#  include <entt/entt.hpp>

#endif

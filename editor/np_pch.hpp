#if !defined(NP_EDITOR_PCH_HPP)
#  define NP_EDITOR_PCH_HPP

#  include <array>
#  include <cstdint>
#  include <cstdio>
#  include <cstdlib>

#  if defined(OS_WINDOWS)
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#  endif

#  if defined(OS_WINDOWS)
#    define VK_USE_PLATFORM_WIN32_KHR
#  elif defined(OS_LINUX)
#    define VK_USE_PLATFORM_WAYLAND_KHR
#  endif

#  include <vulkan/vulkan.h>

#  include <GLFW/glfw3.h>

#endif

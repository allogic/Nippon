#if !defined(NP_EDITOR_MACRO_HPP)
#  define NP_EDITOR_MACRO_HPP

#  if defined(BUILD_DEBUG)
#    if defined(OS_WINDOWS)
#      include <intrin.h>
#      define DEBUG_BREAK() __debugbreak()
#    elif defined(OS_LINUX)
#      include <signal.h>
#      define DEBUG_BREAK() raise(SIGTRAP)
#    endif
#  else
#    define DEBUG_BREAK()
#  endif

#  if defined(BUILD_DEBUG)
#    define VK_CHECK(EXPRESSION)                              \
      {                                                       \
        VkResult result = (EXPRESSION);                       \
        if (result != VK_SUCCESS) {                           \
          printf("%s failed with %d\n", #EXPRESSION, result); \
          DEBUG_BREAK();                                      \
        }                                                     \
      }
#  else
#    define VK_CHECK(EXPRESSION, ...) (EXPRESSION)
#  endif

#endif

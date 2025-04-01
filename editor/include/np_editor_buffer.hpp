#if !defined(NP_EDITOR_BUFFER_HPP)
#  define NP_EDITOR_BUFFER_HPP

struct NpTimeInfo {
};

struct NpScreenInfo {
};

struct NpCameraInfo {
};

struct NpDefaultObjectVertex {
  glm::fvec3 Position;
  glm::fvec4 Color;
};

typedef uint32_t NpDefaultObjectIndex;

struct NpDebugLineVertex {
  glm::fvec3 Position;
  glm::fvec4 Color;
};

typedef uint32_t NpDebugLineIndex;

#endif

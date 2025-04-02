#if !defined(NP_EDITOR_BUFFER_HPP)
#  define NP_EDITOR_BUFFER_HPP

struct NpTimeInfo {
  float Time;
  float DeltaTime;
};

struct NpScreenInfo {
  float Width;
  float Height;
};

struct NpCameraInfo {
  glm::fvec3 WorldPosition;
  float Reserved;
  glm::fmat4 View;
  glm::fmat4 Projection;
  glm::fmat4 ViewProjection;
  glm::fmat4 ViewProjectionInv;
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

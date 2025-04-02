#if !defined(NP_EDITOR_CAMERA_HPP)
#  define NP_EDITOR_CAMERA_HPP

class NpCamera {
public:
  NpCamera(float Fov, float NearZ, float FarZ);
  virtual ~NpCamera();

public:
  inline auto const &GetFov() const { return m_Fov; }
  inline auto const &GetNearZ() const { return m_NearZ; }
  inline auto const &GetFarZ() const { return m_FarZ; }

  inline void SetFov(float Fov) { m_Fov = Fov; }
  inline void SetNearZ(float NearZ) { m_NearZ = NearZ; }
  inline void SetFarZ(float FarZ) { m_FarZ = FarZ; }

private:
  float m_Fov = 0.0F;
  float m_NearZ = 0.0F;
  float m_FarZ = 0.0F;
};

#endif

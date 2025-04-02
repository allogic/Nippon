#include <np_editor_pch.hpp>
#include <np_editor_camera.hpp>

NpCamera::NpCamera(float Fov, float NearZ, float FarZ) : m_Fov(Fov), m_NearZ(NearZ), m_FarZ(FarZ) {}
NpCamera::~NpCamera() {}

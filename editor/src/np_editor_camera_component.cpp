#include <np_editor_pch.hpp>
#include <np_editor_camera_component.hpp>

NpCameraComponent::NpCameraComponent() {}
NpCameraComponent::NpCameraComponent(float Fov, float NearZ, float FarZ) : m_Fov(Fov), m_NearZ(NearZ), m_FarZ(FarZ) {}
NpCameraComponent::~NpCameraComponent() {}

#if !defined(NP_EDITOR_TRANSFORM_SYSTEM_HPP)
#  define NP_EDITOR_TRANSFORM_SYSTEM_HPP

#  include <np_editor_transform_component.hpp>

class NpTransformSystem {
public:
  static void Update(entt::registry &Registry);
};

#endif

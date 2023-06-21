#include <Editor/Scene.h>

#include <Editor/Actors/Box.h>

#include <Editor/Components/Transform.h>

#include <Editor/Renderer/DebugRenderer.h>

///////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////

extern ark::Scene* gScene;

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  Box::Box(std::string const& Name)
    : Actor{ Name }
  {

  }

  void Box::Update(R32 TimeDelta)
  {
    auto& debugRenderer = gScene->GetDebugRenderer();

    debugRenderer.DebugLine(GetTransform()->GetWorldPosition(), GetTransform()->GetWorldPosition() + GetTransform()->GetWorldRight() * 2.0F, R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
    debugRenderer.DebugLine(GetTransform()->GetWorldPosition(), GetTransform()->GetWorldPosition() + GetTransform()->GetWorldUp() * 2.0F, R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
    debugRenderer.DebugLine(GetTransform()->GetWorldPosition(), GetTransform()->GetWorldPosition() + GetTransform()->GetWorldFront() * 2.0F, R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });

    debugRenderer.DebugLine(GetTransform()->GetWorldPosition(), GetTransform()->GetWorldPosition() + GetTransform()->GetLocalRight() * 2.0F, R32V4{ 1.0F, 0.0F, 0.0F, 1.0F });
    debugRenderer.DebugLine(GetTransform()->GetWorldPosition(), GetTransform()->GetWorldPosition() + GetTransform()->GetLocalUp() * 2.0F, R32V4{ 0.0F, 1.0F, 0.0F, 1.0F });
    debugRenderer.DebugLine(GetTransform()->GetWorldPosition(), GetTransform()->GetWorldPosition() + GetTransform()->GetLocalFront() * 2.0F, R32V4{ 0.0F, 0.0F, 1.0F, 1.0F });

    if (GetParent())
    {
      debugRenderer.DebugLine(GetTransform()->GetWorldPosition(), GetParent()->GetTransform()->GetWorldPosition(), R32V4{ 1.0F, 1.0F, 1.0F, 1.0F });
    }

    debugRenderer.DebugBox(GetTransform()->GetWorldPosition(), GetTransform()->GetLocalScale(), R32V4{ 1.0F, 1.0F, 0.0F, 1.0F }, GetTransform()->GetWorldRotation());
  }
}
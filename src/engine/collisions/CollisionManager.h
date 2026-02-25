#pragma once
#include <map>
#include <string>
#include <vector>

namespace SSGE
{
class GameObject;
}
class CollisionManager
{
  public:
    CollisionManager()
    {
    }
    ~CollisionManager()
    {
    }

    void checkCollisions();

    void addGameObjectCollider(SSGE::GameObject *gameObject);

    void removeGameObjectCollider(SSGE::GameObject *gameObject);

    auto clear() -> void;

  private:
    std::vector<SSGE::GameObject *> m_primaryColliders;
    std::map<std::string, std::vector<SSGE::GameObject *>> m_collidersByLayer;
};

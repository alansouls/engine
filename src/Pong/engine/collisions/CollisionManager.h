#pragma once
#include <map>
#include <memory>
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

    void addGameObjectCollider(const std::shared_ptr<SSGE::GameObject> &gameObject);

    void removeGameObjectCollider(const std::shared_ptr<SSGE::GameObject> &gameObject);

  private:
    std::vector<std::shared_ptr<SSGE::GameObject>> m_primaryColliders;
    std::map<std::string, std::vector<std::shared_ptr<SSGE::GameObject>>> m_collidersByLayer;
};
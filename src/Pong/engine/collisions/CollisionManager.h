#pragma once
#include <map>
#include <string>
#include <vector>
#include <memory>

class GameObject;
class CollisionManager
{
public:
	CollisionManager() {}
	~CollisionManager() {}

	void checkCollisions();

	void addGameObjectCollider(const std::shared_ptr<GameObject>& gameObject);

	void removeGameObjectCollider(const std::shared_ptr<GameObject>& gameObject);

private:
	std::vector<std::shared_ptr<GameObject>> m_primaryColliders;
	std::map<std::string, std::vector<std::shared_ptr<GameObject>> > m_collidersByLayer;
};
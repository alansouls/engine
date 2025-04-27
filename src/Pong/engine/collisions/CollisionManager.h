#pragma once
#include <map>
#include <string>
#include <vector>

class GameObject;
class CollisionManager
{
public:
	CollisionManager() {}
	~CollisionManager() {}

	void checkCollisions();

	void addGameObjectCollider(GameObject* gameObject);

	void removeGameObjectCollider(GameObject* gameObject);

private:
	std::vector<GameObject*> m_primaryColliders;
	std::map<std::string, std::vector<GameObject*> > m_collidersByLayer;
};
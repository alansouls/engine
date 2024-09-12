#pragma once
#include <vector>

class GameObject;
class CollisionManager
{
public:
	CollisionManager() {}
	~CollisionManager() {}

	void checkCollisions(const std::vector<GameObject* > &gameObjects);
};
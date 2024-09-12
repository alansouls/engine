#include <string>
#include <vector>

class CollisionManager;
class Renderer;
class GameObject;
class Scene
{
public:
    Scene(const std::string &name, Renderer* renderer, CollisionManager* collisionManager);
    ~Scene();

    void addGameObject(GameObject *gameObject);
    void removeGameObject(GameObject *gameObject);

    void run();

	const std::string& getName() const;

	void onKeyPressed(int key);
	void onKeyReleased(int key);
	void onKeyDown(int key);
private:
    std::string m_name;
    std::vector<GameObject *> m_gameObjects;
    Renderer * m_renderer;
    CollisionManager* m_collisionManager;
};
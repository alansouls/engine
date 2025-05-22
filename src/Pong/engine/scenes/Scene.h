#include <string>
#include <vector>
#include <memory>

class CollisionManager;
class Renderer;
class GameObject;
class Scene
{
public:
    Scene(const std::string &name, Renderer* renderer, CollisionManager* collisionManager);
    ~Scene();

    void addGameObject(const std::shared_ptr<GameObject> &gameObject);
    void removeGameObject(const std::shared_ptr<GameObject> &gameObject);

    void run();

	const std::string& getName() const;

	void onKeyPressed(int key);
	void onKeyReleased(int key);
	void onKeyDown(int key);
private:
    std::string m_name;
    std::vector<std::shared_ptr<GameObject>> m_gameObjects;
    Renderer * m_renderer;
    CollisionManager* m_collisionManager;
};
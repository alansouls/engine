#include <string>
#include <vector>

class Renderer;
class GameObject;
class Scene
{
public:
    Scene(const std::string &name, Renderer* renderer);
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
};
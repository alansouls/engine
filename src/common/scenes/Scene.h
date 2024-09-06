#include <string>
#include <vector>
#include "GameObject.h"
#include "../graphics/renderers/Renderer.h"

class Scene
{
public:
    Scene(const std::string &name, Renderer* renderer);
    ~Scene();

    void addGameObject(GameObject *gameObject);
    void removeGameObject(GameObject *gameObject);

    void run();
private:
    std::string m_name;
    std::vector<GameObject *> m_gameObjects;
    Renderer * m_renderer;
};
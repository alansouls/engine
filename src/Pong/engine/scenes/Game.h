#include "../graphics/renderers/Renderer.h";
#include "GameProperties.h"
#include <GLFW/glfw3.h>
#include <string>

class Scene;
class Game
{
public:
    Game(GLFWwindow* window, Renderer* renderer);
    virtual ~Game() = 0;
    
    virtual void setup() = 0;
    
    void run();

	static Game* getInstance();
	static void setInstance(Game* instance);

	Scene* addScene(const std::string &name);
	void removeScene(const std::string& name);
	void setCurrentScene(const std::string& name);

	Scene* getCurrentScene() const;

    GameProperties getProperties() const;
private:
    Renderer *m_renderer;
    
    std::vector<Scene*> m_scenes;
    Scene* m_currentScene;

	GLFWwindow* m_window;

    static Game* m_instance;
};

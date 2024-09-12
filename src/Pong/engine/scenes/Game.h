#include "../graphics/renderers/Renderer.h";
#include "GameProperties.h"
#include <GLFW/glfw3.h>
#include <string>

class CollisionManager;
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

	void pause();
	void resume();

	bool isPaused() const;
    
protected:
    virtual void onKeyPressed(int key);
    virtual void onKeyReleased(int key);
	virtual void onKeyDown(int key);
private:
    Renderer *m_renderer;
    
    std::vector<Scene*> m_scenes;
    Scene* m_currentScene;

	GLFWwindow* m_window;

    bool m_paused;

    static Game* m_instance;

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	CollisionManager* m_collisionManager;
};

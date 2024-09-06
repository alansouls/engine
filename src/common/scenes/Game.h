#include "../graphics/renderers/Renderer";

class Game
{
public:
    Game(Renderer* renderer);
    virtual ~Game() = 0;
    
    virtual void setup() = 0;
    
    void run();
private:
    Renderer *renderer;
    
    std::vec
};

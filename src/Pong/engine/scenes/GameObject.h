#include "GameProperties.h"
class RendererItem;
class GameObject
{
public:
    GameObject();
    virtual ~GameObject() = 0;
    virtual void init() = 0;
    virtual void update() = 0;

    RendererItem* getRendererItem() const;
protected:
	void setRendererItem(RendererItem* rendererItem);

    GameProperties getGameProperties();

private:
    RendererItem* m_rendererItem;
};

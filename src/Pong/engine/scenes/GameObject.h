#pragma once

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

    virtual void onKeyPressed(int key);
	virtual void onKeyReleased(int key);
	virtual void onKeyDown(int key);
protected:
	void setRendererItem(RendererItem* rendererItem);

    GameProperties getGameProperties();

private:
    RendererItem* m_rendererItem;
};

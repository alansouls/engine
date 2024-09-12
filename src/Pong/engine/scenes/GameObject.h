#pragma once

#include "GameProperties.h"

class Collider;
class CollisionInfo;
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

	virtual void onCollisionEnter(const CollisionInfo &info);
    virtual void onCollisionExit(const CollisionInfo &info);

    Collider* getCollider() const;
protected:
	void setRendererItem(RendererItem* rendererItem);

    GameProperties getGameProperties();

	void setCollider(Collider* collider);
private:
    RendererItem* m_rendererItem;
    Collider* m_collider;
};

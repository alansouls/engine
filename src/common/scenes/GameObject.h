class RendererItem;
class GameObject
{
public:
    virtual ~GameObject() = 0;
    virtual void init() = 0;
    virtual void update() = 0;

    RendererItem* getRendererItem() const;
private:
    RendererItem* m_rendererItem;
};

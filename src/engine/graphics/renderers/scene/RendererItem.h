#pragma once

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>

struct TransformUpdatedCallback
{
    void *caller;
    void (*callback)(void *, uint32_t);

    [[nodiscard]] bool isSameAs(const TransformUpdatedCallback &other) const
    {
        return caller == other.caller && callback == other.callback;
    }
};

enum RendererItemType : uint32_t
{
    Triangle,
    Rectangle,
    Circle,
    RendererItemTypeCount
};

class RendererItem
{
  public:
    virtual ~RendererItem() = default;

    virtual void updateTransform() = 0;

    glm::vec3 getTransformPosition() const
    {
        return m_transformPosition;
    }

    glm::vec3 getTransformScale() const
    {
        return m_transformScale;
    }

    void setTransform(glm::vec3 position, glm::vec3 scale)
    {
        m_transformPosition = position;
        m_transformScale = scale;
        executeTransformUpdatedCallbacks();
    }

    void setPosition(glm::vec3 position)
    {
        setTransform(position, m_transformScale);
    }

    void setScale(glm::vec3 scale)
    {
        setTransform(m_transformPosition, scale);
    }

    uint32_t getKey() const
    {
        return m_key;
    }

    void setKey(uint32_t key)
    {
        m_key = key;
    }

    void addCallback(void *callerPtr, void (*callback)(void *, uint32_t))
    {
        m_transformUpdatedCallbacks.push_back({callerPtr, callback});
    }

    void removeCallback(void *callerPtr, void (*callback)(void *, uint32_t))
    {
        TransformUpdatedCallback toRemove = {callerPtr, callback};
        auto iter = m_transformUpdatedCallbacks.begin();
        while (iter != m_transformUpdatedCallbacks.end())
        {
            if ((*iter).isSameAs(toRemove))
            {
                m_transformUpdatedCallbacks.erase(iter);
                return;
            }
            iter++;
        }
    }

    [[nodiscard]] RendererItemType getType() const
    {
        return m_type;
    }

    auto bindWorldTransform(const glm::mat4 &transform)
    {
        m_worldTransform = &transform;
    }

    [[nodiscard]] auto getWorldTransform() const -> const glm::mat4 &
    {
        return *m_worldTransform;
    }

    [[nodiscard]] auto getFillColor() const -> const glm::vec4 &
    {
        return m_fillColor;
    }

    auto setFillColor(const glm::vec4 &fillColor) -> void
    {
        m_fillColor = fillColor;
    }

  protected:
    explicit RendererItem(RendererItemType type)
        : m_key(0), m_type(type), m_worldTransform(nullptr), m_transformPosition(glm::vec3(0.0f)),
          m_transformScale(glm::vec3(1.0f)), m_fillColor()
    {
    }

  private:
    uint32_t m_key;
    RendererItemType m_type;

    const glm::mat4 *m_worldTransform;
    glm::vec3 m_transformPosition;
    glm::vec3 m_transformScale;
    glm::vec4 m_fillColor;

    std::vector<TransformUpdatedCallback> m_transformUpdatedCallbacks;

    void executeTransformUpdatedCallbacks()
    {
        for (auto callback : m_transformUpdatedCallbacks)
        {
            callback.callback(callback.caller, m_key);
        }
    }
};

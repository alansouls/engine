#pragma once
#include <cstdint>

class UIView
{
  public:
    virtual ~UIView() = default;
    virtual auto render(uint32_t currentImage) -> void = 0;

    [[nodiscard]] auto getWidth() const -> uint32_t
    {
        return m_width;
    }

    [[nodiscard]] auto getHeight() const -> uint32_t
    {
        return m_height;
    }

protected:
    uint32_t m_width = 500;
    uint32_t m_height = 500;
};

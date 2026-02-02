#pragma once
#include <cstdint>
#include <string>

#include "../UIMessenger.h"

class UIView
{
  public:
    UIView(const std::string &name, SSGE::UIMessenger *messenger) : m_name(name), m_open(false), m_messenger(messenger)
    {
    }

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

    auto getOpen() const -> bool
    {
        return m_open;
    }

    auto setOpen(bool open) -> void
    {
        m_open = open;
    }

    auto getName() const -> const std::string &
    {
        return m_name;
    }

  protected:
    auto sendMessage(const std::string &name, void *data) -> void
    {
        if (m_messenger)
            m_messenger->send(name, data);
    }

  protected:
    std::string m_name;
    uint32_t m_width = 500;
    uint32_t m_height = 500;
    bool m_open;

  private:
    SSGE::UIMessenger *m_messenger;
};

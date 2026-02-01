//
// Created by Alan Maia on 01/02/2026.
//

#include "UIMessenger.h"

namespace SSGE
{
    auto UIMessenger::connect(const std::string& name, const std::function<auto(void* data) -> void>& receiver) -> void
    {
        auto& receivers = m_receivers[name];
        receivers.push_back(receiver);
    }

    auto UIMessenger::send(const std::string& name, void* data) -> void
    {
        for (auto& receiver : m_receivers[name])
        {
            receiver(data);
        }
    }
} // SSGE

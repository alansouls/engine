#pragma once
#include <functional>
#include <unordered_map>

namespace SSGE
{
    class UIMessenger
    {
    public:
        explicit UIMessenger() = default;

        auto connect(const std::string& name, const std::function<auto (void* data) -> void>& receiver) -> void;

        auto send(const std::string& name, void* data) -> void;

    private:
        std::unordered_map<std::string, std::vector<std::function<auto (void* data) -> void>>> m_receivers;
    };
} // SSGE

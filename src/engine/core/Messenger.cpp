#include "Messenger.h"

#include <algorithm>
#include <ranges>
#include <utility>

namespace SSGE
{

auto Messenger::disconnect(ConnectionOwner owner) -> void
{
    for (auto &receivers : m_receivers | std::views::values)
    {
        receivers.erase(owner);
    }
}

auto Messenger::connect(ConnectionOwner owner, const std::string_view &name,
                        std::function<auto(void *data)->void> receiver) -> void
{
    auto &receivers = m_receivers[std::string(name)];
    receivers.insert(std::make_pair(owner, receiver));
}

auto Messenger::send(const std::string_view &name, void *data) -> void
{
    const auto receivers = m_receivers[std::string(name)];

    for (const std::function<void(void *)> &receiver : receivers | std::views::values)
    {
        receiver(data);
    }
}

} // namespace SSGE

#include "Messenger.h"

namespace SSGE
{

auto Messenger::connect(const std::string_view &name, std::function<auto(void *data)->void> receiver) -> void
{
    auto &receivers = m_receivers[std::string(name)];
    receivers.push_back(std::move(receiver));
}

auto Messenger::send(const std::string_view &name, void *data) const -> void
{
    const auto receivers = m_receivers.find(std::string(name));

    if (receivers == m_receivers.end())
        return;

    for (const std::function<void(void *)> &receiver : receivers->second)
    {
        receiver(data);
    }
}

} // namespace SSGE
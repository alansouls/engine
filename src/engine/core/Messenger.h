#pragma once
#include <cstdint>
#include <functional>
#include <map>
#include <string>

namespace SSGE
{

template <class TMessage>
concept Message = requires(TMessage &message) {
    { TMessage::Name } -> std::convertible_to<std::string_view>;
};

typedef void *ConnectionOwner;

class Messenger
{
  public:
    auto disconnect(ConnectionOwner owner) -> void;

    template <Message TMessage>
    auto connect(ConnectionOwner owner, const std::function<void(const TMessage &)> &callback) -> void;

    template <Message TMessage> auto send(const TMessage &message) -> void;

    auto connect(ConnectionOwner owner, const std::string_view &name, std::function<auto(void *data)->void> receiver)
        -> void;
    auto send(const std::string_view &name, void *data) -> void;

  private:
    std::unordered_map<std::string, std::map<ConnectionOwner, std::function<auto(void *data)->void>>> m_receivers;
};

template <Message TMessage>
auto Messenger::connect(ConnectionOwner owner, const std::function<void(const TMessage &)> &callback) -> void
{
    connect(owner, TMessage::Name, [callback](void *data) { callback(*static_cast<TMessage *>(data)); });
}

template <Message TMessage> auto Messenger::send(const TMessage &message) -> void
{
    send(TMessage::Name, const_cast<TMessage *>(&message));
}

} // namespace SSGE

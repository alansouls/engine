#include "SceneSerializerV1.h"

#include <vector>
#include <ranges>
#include <iostream>
#include <array>

namespace SSGE
{

auto SceneSerializerV1::serialize(std::ostream &stream, const SceneDefinition &definition) -> void
{
}

auto SceneSerializerV1::deserialize(std::istream &stream) -> SceneDefinition
{
    constexpr uint32_t nameChunk = 255;
    std::string name;
    std::vector<uint8_t> nameBytes;
    nameBytes.reserve(nameChunk);
    while (name.empty())
    {
        std::array<uint8_t, nameChunk> nameBuffer = {};
        uint32_t bytesRead = stream.readsome(reinterpret_cast<char *>(nameBuffer.data()), nameChunk);
        if (bytesRead == 0)
        {
            throw std::runtime_error("Could not read scene name from stream");
        }
        auto begin = nameBuffer.begin();
        auto nullTerminator = std::ranges::find(nameBuffer, 0);
        while (begin != nullTerminator)
        {
            nameBytes.push_back(*begin++);
        }

        if (nullTerminator != nameBuffer.end())
        {
            name = std::string(nameBytes.begin(), nameBytes.end());
            break;
        }
    }

    return SceneDefinition{
        .name = std::move(name),
        .gameObjects = {},
    };
}

auto SceneSerializerV1::version() const -> std::array<uint8_t, 3>
{
    return {1, 0, 0};
}

} // SSGE
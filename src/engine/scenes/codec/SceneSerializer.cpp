#include "SceneSerializer.h"

#include "versions/SceneSerializerV1.h"

#include <array>

namespace SSGE
{

SceneSerializer::Version SceneSerializer::s_cachedSerializerVersion = Version::LATEST;
std::unique_ptr<BaseSceneSerializer> SceneSerializer::s_cachedSerializer = nullptr;

auto SceneSerializer::serialize(std::ostream &stream, const SceneDefinition &definition, Version version) -> void
{
    getSerializerForVersion(version)->serialize(stream, definition);
}

auto SceneSerializer::deserialize(std::istream &stream) -> SceneDefinition
{
    std::array<uint8_t, 3> versionBuffer = {0};
    uint32_t bytesRead = 3;
    if (!stream.read(reinterpret_cast<char *>(versionBuffer.data()), bytesRead))
    {
        throw std::runtime_error("Could not read file version from stream");
    }
    auto version = static_cast<Version>((versionBuffer[0] << 16) | (versionBuffer[1] << 8) | versionBuffer[2]);

    return getSerializerForVersion(version)->deserialize(stream);
}

auto SceneSerializer::getSerializerForVersion(Version version) -> BaseSceneSerializer *
{
    if (s_cachedSerializer != nullptr && version == s_cachedSerializerVersion)
    {
        return s_cachedSerializer.get();
    }

    s_cachedSerializerVersion = version;

    switch (version)
    {
    case Version::V1:
    case Version::LATEST:
        s_cachedSerializer = std::make_unique<SceneSerializerV1>();
        break;
    default:
        throw std::runtime_error("Unsupported scene version: " + std::to_string(static_cast<int>(version)));
    }

    return s_cachedSerializer.get();
}
}
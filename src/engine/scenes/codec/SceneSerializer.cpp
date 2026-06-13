#include "SceneSerializer.h"

#include "versions/SceneSerializerV1.h"

#include <cmath>
#include <format>
#include <stdexcept>

namespace SSGE
{

SceneSerializer::Version SceneSerializer::s_cachedSerializerVersion = Version::LATEST;
std::unique_ptr<BaseSceneSerializer> SceneSerializer::s_cachedSerializer = nullptr;

auto SceneSerializer::serialize(std::ostream &stream, const SceneDefinition &definition, Version version) -> void
{
    std::string versionString = versionToString(version);
    stream << versionString << "\r\n";
    getSerializerForVersion(version)->serialize(stream, definition);
}

auto SceneSerializer::deserialize(std::istream &stream) -> SceneDefinition
{
    char versionBuffer[3] = {};
    if (!stream.read(versionBuffer, sizeof(versionBuffer)))
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

auto SceneSerializer::versionToString(Version version) -> std::string
{
    int32_t versionInt = static_cast<int32_t>(version);
    int8_t major = versionInt >> 16;
    int8_t minor = (versionInt >> 8) & 0x000011;
    int8_t patch = versionInt & 0x000011;

    return std::format("{}.{}.{}", major, minor, patch);
}

auto SceneSerializer::versionFromString(const std::string_view &versionString) -> Version
{
    int8_t parts[3] = {};
    int readDigits = 0;
    int part = 0;
    for (size_t i = 0; i < versionString.length(); ++i)
    {
        char current = versionString[i];
        if ((readDigits == 3 && current != '.') || (current == '.' && readDigits == 0) || part > 2)
        {
            throw std::runtime_error("Invalid version string!");
        }
        if (current >= '0' && current <= '9')
        {
            parts[part] = (current - '0') * std::pow(10, 2 - readDigits);
            ++readDigits;
        }
        else if (current == '.')
        {
            part++;
            readDigits = 0;
        }
        else
        {
            throw std::runtime_error("Invalid version string!");
        }
    }

    int32_t versionInt = (parts[0] << 16) | (parts[1] << 8) | parts[2];
    return static_cast<Version>(versionInt);
}

} // namespace SSGE

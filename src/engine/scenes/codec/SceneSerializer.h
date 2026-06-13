#pragma once

#include "scenes/SceneDefinitions.h"
#include "versions/BaseSceneSerializer.h"

#include <iostream>
#include <memory>
#include <string>
#include <string_view>

namespace SSGE
{
class SceneSerializer
{
  public:
    enum class Version
    {
        LATEST = -1,
        V1 = 0x010000,
    };

    static auto serialize(std::ostream &stream, const SceneDefinition &definition, Version version = Version::LATEST)
        -> void;

    static auto deserialize(std::istream &stream) -> SceneDefinition;

    static auto versionToString(Version version) -> std::string;

    static auto versionFromString(const std::string_view &versionString) -> Version;

  private:
    static Version s_cachedSerializerVersion;
    static std::unique_ptr<BaseSceneSerializer> s_cachedSerializer;
    static auto getSerializerForVersion(Version version) -> BaseSceneSerializer *;
};
} // namespace SSGE

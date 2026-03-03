#pragma once

#include "scenes/SceneDefinitions.h"
#include "versions/BaseSceneSerializer.h"

#include <iostream>

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

    static auto serialize(std::ostream &stream, const SceneDefinition &definition,
                          Version version = Version::LATEST) -> void;

    static auto deserialize(std::istream &stream) -> SceneDefinition;

private:
    static Version s_cachedSerializerVersion;
    static std::unique_ptr<BaseSceneSerializer> s_cachedSerializer;
    static auto getSerializerForVersion(Version version) -> BaseSceneSerializer *;
};
}
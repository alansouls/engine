#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>

namespace SSGE
{

template <typename TDataType>
concept ParsableType =
    std::same_as<TDataType, int> || std::same_as<TDataType, float> || std::same_as<TDataType, std::string> ||
    std::same_as<TDataType, bool> || std::same_as<TDataType, glm::vec2> || std::same_as<TDataType, glm::vec3> ||
    std::same_as<TDataType, glm::vec4>;

class ParseUtils
{
  public:
    template <ParsableType TParseType> static auto Parse(const std::string_view &value) -> TParseType;

  private:
    static constexpr char TrueChar = 'T';
};

} // namespace SSGE

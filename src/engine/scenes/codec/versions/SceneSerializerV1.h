#pragma once
#include "BaseSceneSerializer.h"

#include <array>
#include <glm/vec3.hpp>
#include <iostream>
#include <span>
#include <string>
#include <vector>

namespace SSGE
{
class SceneSerializerV1 : public BaseSceneSerializer
{
    class StreamReader
    {
      public:
        StreamReader(std::istream &stream) : m_stream(stream), m_pos(0), m_pageSize(0)
        {
        }

        auto returnPos(int32_t offset) -> void
        {
            auto prevPos = m_pos;
            m_pos = std::max(m_pos - offset, 0);
            m_pageSize = prevPos - m_pos;
        }

        auto readNext(int32_t &bytesRead) -> std::span<std::byte>
        {
            bytesRead = std::min(bytesRead, BufferSize);
            if (bytesRead + m_pos > BufferSize)
            {
                bytesRead = BufferSize - m_pos;
                std::move(m_pageBuffer.begin() + m_pos, m_pageBuffer.end(), m_pageBuffer.begin());
                m_pos = 0;
            }

            auto prevPos = m_pos;
            std::streamsize readFromStream = std::max(bytesRead - m_pageSize, 0);
            if (readFromStream != 0)
            {
                m_stream.read(reinterpret_cast<char *>(m_pageBuffer.data()) + m_pos + m_pageSize, readFromStream);
                bytesRead = static_cast<int32_t>(m_stream.gcount()) + m_pageSize;
            }
            m_pageSize = std::max(m_pageSize - bytesRead, 0);
            m_pos += bytesRead;
            m_pos %= BufferSize;
            return {m_pageBuffer.begin() + prevPos, m_pageBuffer.begin() + prevPos + bytesRead};
        }

      private:
        static constexpr int32_t BufferSize = 1024;

        std::istream &m_stream;
        int32_t m_pos;
        int32_t m_pageSize;
        std::array<std::byte, BufferSize> m_pageBuffer{};
    };

  public:
    ~SceneSerializerV1() override = default;

    auto serialize(std::ostream &stream, const SceneDefinition &definition) -> void override;
    auto deserialize(std::istream &stream) -> SceneDefinition override;
    [[nodiscard]] auto version() const -> std::array<uint8_t, 3> override;

  protected:
    virtual auto deserializeGameObjects(StreamReader &stream) -> std::vector<GameObjectDefinition>;
    virtual auto deserializeGameObject(StreamReader &stream) -> GameObjectDefinition;
    virtual auto deserializeComponents(StreamReader &stream) -> std::vector<ComponentDefinition>;
    virtual auto deserializeComponent(StreamReader &stream) -> ComponentDefinition;
    virtual auto deserializeComponentFields(StreamReader &stream) -> std::vector<ComponentFieldDefinition>;
    virtual auto deserializeComponentField(StreamReader &stream) -> ComponentFieldDefinition;
    virtual auto deserializeTransform(StreamReader &stream) -> TransformDefinition;
    virtual auto deserializeUInt32T(StreamReader &stream, const std::string_view &fieldName) -> uint32_t;
    virtual auto deserializeString(StreamReader &stream, const std::string_view &fieldName) -> std::string;
    virtual auto deserializeVector3(StreamReader &stream, const std::string_view &fieldName) -> glm::vec3;
};
} // namespace SSGE

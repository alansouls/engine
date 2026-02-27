#pragma once
#include "UIView.h"
#include "scenes/ComponentField.h"
#include "scenes/GameObject.h"

namespace SSGE
{
class InspectorView : public UIView
{
  public:
    explicit InspectorView(Messenger *messenger);

    auto render(uint32_t currentImage) -> void override;

  private:
    GameObject *m_selectedGameObject;

    class InspectorComponents
    {
      public:
        static auto GameObjectComponents(GameObject *gameObject) -> void;

      private:
        static auto TransformComponent(GameObject *gameObject) -> void;
        static void FieldInputInt(TypedComponentField<int> *field);
        static void FieldInputFloat(TypedComponentField<float> *field);
        static void FieldInputText(TypedComponentField<std::string> *field);
        static void FieldInputBool(TypedComponentField<bool> *field);
        static void FieldInputVec2(TypedComponentField<glm::vec2> *field);
        static void FieldInputVec3(TypedComponentField<glm::vec3> *field);
        static void FieldInputVec4(TypedComponentField<glm::vec4> *field);
        static void FieldInputColor(TypedComponentField<glm::vec4> *field);
        static auto GenericComponent(GameObject *gameObject, Component *component) -> void;
    };
};
} // namespace SSGE

#pragma once
#include "UIView.h"
#include "scenes/GameObject.h"

namespace SSGE
{
class InspectorView : public UIView
{
  public:
    explicit InspectorView(UIMessenger *messenger);

    auto render(uint32_t currentImage) -> void override;

  private:
    GameObject *m_selectedGameObject;

    class InspectorComponents
    {
      public:
        static auto GameObjectComponents(GameObject *gameObject) -> void;

      private:
        static auto TransformComponent(GameObject *gameObject) -> void;
        static void FieldInputInt(ComponentField *field);
        static void FieldInputFloat(ComponentField *field);
        static void FieldInputText(ComponentField *field);
        static void FieldInputBool(ComponentField *field);
        static void FieldInputVec2(ComponentField *field);
        static void FieldInputVec3(ComponentField *field);
        static void FieldInputVec4(ComponentField *field);
        static void FieldInputColor(ComponentField *field);
        static auto GenericComponent(GameObject *gameObject, Component *component) -> void;
    };
};
} // namespace SSGE

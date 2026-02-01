#pragma  once
#include "UIView.h"
#include "scenes/GameObject.h"

namespace SSGE
{
    class InspectorView : public UIView
    {
    public:
        explicit InspectorView(UIMessenger* messenger);

        auto render(uint32_t currentImage) -> void override;

    private:
        SSGE::GameObject* m_selectedGameObject;
    };
} // SSGEEditor

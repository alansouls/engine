#pragma once

#include "UIView.h"
#include "vulkan/vulkan_core.h"

struct SceneImage
{
    VkDescriptorSet descriptorSet;
    uint32_t width;
    uint32_t height;
};

class SceneView : public UIView
{
  public:
    SceneView(const SceneImage &image);
    auto render() -> void override;

  private:
    SceneImage m_sceneImage;
};

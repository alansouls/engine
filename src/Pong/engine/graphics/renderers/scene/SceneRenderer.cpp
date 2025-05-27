#include "SceneRenderer.h"
#include "../../drivers/GraphicsOperation.h"
#include "CircleItem.h"
#include "RectangleItem.h"
#include "RendererItem.h"

SceneRenderer::SceneRenderer(VulkanDriver *driver) : m_driver(driver)
{
}

auto SceneRenderer::render() -> void
{
    auto addOrRemoveOperations = getAddOrRemoveOperations();

    if (!addOrRemoveOperations.empty())
        m_driver->waitIdle();

    for (auto item : addOrRemoveOperations)
    {
        auto operation = item.second;
        auto renderItem = item.first;
        m_driver->performOperation(&operation);

        if (!operation.result.has_value() || operation.result.value() < 0)
            continue;

        if (operation.type == GraphicsOperation::Type::Add)
        {
            renderItem->setKey(operation.result.value());
            m_items.insert(std::make_pair(operation.result.value(), renderItem));
        }
        else if (operation.type == GraphicsOperation::Type::Remove)
        {
            m_items.erase(operation.key);
        }
    }

    std::vector<GraphicsOperation> updateOperations = getUpdateOperations();
    std::vector<GraphicsOperation *> operations;
    for (auto &operation : updateOperations)
    {
        operations.push_back(&operation);
    }

    // render scene texture
    //
}

std::map<RendererItem *, GraphicsOperation> SceneRenderer::getAddOrRemoveOperations()
{
    std::map<RendererItem *, GraphicsOperation> addOrRemoveOperations;
    if (m_addedSet.empty() && m_removedSet.empty())
        return addOrRemoveOperations;

    for (RendererItem *added : m_addedSet)
    {
        GraphicsOperation operation;
        operation.type = GraphicsOperation::Type::Add;
        operation.transformPosition = added->getTransformPosition();
        operation.transformScale = added->getTransformScale();
        switch (added->getType())
        {
        case RendererItem::RendererItemType::Rectangle:
            operation.elementType = GraphicsDriver::ElementType::Quad;
            operation.color = reinterpret_cast<RectangleItem *>(added)->getFillColor();
            break;
        case RendererItem::RendererItemType::Circle:
            operation.elementType = GraphicsDriver::ElementType::Circle;
            operation.color = reinterpret_cast<CircleItem *>(added)->getFillColor();
            break;
        default:
            std::runtime_error("Renderer item type not supported");
        }
        addOrRemoveOperations.insert(std::make_pair(added, operation));
    }

    for (auto removed : m_removedSet)
    {
        GraphicsOperation operation;
        operation.type = GraphicsOperation::Type::Remove;
        operation.key = removed;
        addOrRemoveOperations.insert(std::make_pair(nullptr, operation));
    }

    m_addedSet.clear();
    m_removedSet.clear();

    return addOrRemoveOperations;
}

std::vector<GraphicsOperation> SceneRenderer::getUpdateOperations()
{
    std::vector<GraphicsOperation> updateOperations;

    if (m_updatedSet.empty())
        return updateOperations;

    for (auto key : m_updatedSet)
    {
        auto updated = m_items.find(key)->second;
        if (updated == nullptr)
            continue;

        GraphicsOperation operation;
        operation.type = GraphicsOperation::Type::Update;
        switch (updated->getType())
        {
        case RendererItem::RendererItemType::Rectangle:
            operation.elementType = GraphicsDriver::ElementType::Quad;
            operation.color = reinterpret_cast<RectangleItem *>(updated)->getFillColor();
            break;
        case RendererItem::RendererItemType::Circle:
            operation.elementType = GraphicsDriver::ElementType::Circle;
            operation.color = reinterpret_cast<CircleItem *>(updated)->getFillColor();
            break;
        default:
            throw std::runtime_error("Renderer item type not supported");
        }
        operation.transformPosition = updated->getTransformPosition();
        operation.transformScale = updated->getTransformScale();
        operation.key = key;
        updateOperations.push_back(operation);
    }

    m_updatedSet.clear();

    return updateOperations;
}

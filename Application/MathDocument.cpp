// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <MathDocument.h>

#include <QtGui/QScreen>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGTextureProvider>
#include <QtQuick/QSGSimpleTextureNode>

#include <QVulkanInstance>
#include <QVulkanFunctions>
#include <QFile>

#include <VulkanHelpers.h>
#include <VertexInputLayout.h>
#include <VulkanContext.h>
#include <MathDocumentRendering.h>
#include <FreeTypeWrap.h>

#include <Application.h>
#include <AppGlobal.h>

class CustomTextureNodePrivate : public QSGTextureProvider, public QSGSimpleTextureNode
{
    Q_OBJECT

public:
    CustomTextureNodePrivate(QQuickItem* item);
    ~CustomTextureNodePrivate() override;

    QSGTexture* texture() const override;

    //Synchronization during paint node
    void sync();

    //Cache doc state ptr in node
    void setMeDocState(FMathDocumentState* meDocState);
private slots:
    //Rendering logic
    void render();

private:
    //Creates a texture used by scene graph
    bool buildTexture(const QSize& size);

    //Releases texture resources
    void freeTexture();

    //Renderer initialization logic
    bool initialize();

    //Finds memory type suitable for the buffer
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    //Creates VkBuffer and binds memory
    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    
    //Creates command pool on graphics queue
    void createCommandPool();

    //Creates command buffer for single time command
    VkCommandBuffer BeginSingleTimeCommands();

    //Executes command buffer on graphics queue
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

    //Transits image layout
    void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    
    //Copying VkBuffer into VkImage
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    QQuickItem* m_item = nullptr;
    QQuickWindow* m_window = nullptr;
    QSize m_size = { 0,0 };
    qreal m_dpr = 0;

    VkImage m_texture = VK_NULL_HANDLE;
    VkDeviceMemory m_textureMemory = VK_NULL_HANDLE;

    bool m_initialized = false;

    VkPhysicalDevice m_physDev = VK_NULL_HANDLE;
    VkDevice m_dev = VK_NULL_HANDLE;
    QVulkanDeviceFunctions* m_devFuncs = nullptr;
    QVulkanFunctions* m_funcs = nullptr;

    //Request to destroy vulkan context if Item destroyed
    struct ContextDestroyer
    {
        ~ContextDestroyer()
        {
            FVulkanStatic::UnsubscribeFromContext();
        }
    } m_contextDestroyer;


    FMathDocumentRendering m_documentRendering;
    uint32_t m_graphicsFamily = 0;
    VkCommandPool m_commandPool = nullptr;
    VkQueue m_graphicsQueue = nullptr;
    FMathDocumentState* m_meDocState = nullptr;
};

MathDocument::MathDocument()
{
    setFlag(ItemHasContents, true);
}

void MathDocument::setMeDocState(FMathDocumentState* meDocState)
{
    m_node->setMeDocState(meDocState);
}

bool MathDocument::isNodeCreated()
{
    return m_node != nullptr;
}

void MathDocument::invalidateSceneGraph() // called on the render thread when the scenegraph is invalidated
{
    m_node = nullptr;
}

void MathDocument::releaseResources() // called on the gui thread if the item is removed from scene
{
    m_node = nullptr;
}

QSGNode* MathDocument::updatePaintNode(QSGNode* node, UpdatePaintNodeData*)
{
    CustomTextureNodePrivate* n = static_cast<CustomTextureNodePrivate*>(node);

    if (!n && (width() <= 0 || height() <= 0))
        return nullptr;

    if (!n) {
        m_node = new CustomTextureNodePrivate(this);
        n = m_node;
        emit onNodeCreated();
    }
    m_node->sync();

    n->setTextureCoordinatesTransform(QSGSimpleTextureNode::NoTransform);
    n->setFiltering(QSGTexture::None);
    n->setRect(0, 0, width(), height());

    window()->update(); // ensure getting to beforeRendering() at some point

    return n;
}

void MathDocument::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);

    if (newGeometry.size() != oldGeometry.size())
        update();
}
CustomTextureNodePrivate::CustomTextureNodePrivate(QQuickItem* item)
    : m_item(item)
{
    m_window = m_item->window();
    connect(m_window, &QQuickWindow::beforeRendering, this, &CustomTextureNodePrivate::render);
    connect(m_window, &QQuickWindow::screenChanged, this, [this]() {
        if (m_window->effectiveDevicePixelRatio() != m_dpr)
            m_item->update();
        });
}

CustomTextureNodePrivate::~CustomTextureNodePrivate()
{
    m_devFuncs->vkDestroyCommandPool(m_dev, m_commandPool, nullptr);
    delete texture();
    freeTexture();
}

QSGTexture* CustomTextureNodePrivate::texture() const
{
    return QSGSimpleTextureNode::texture();
}

bool CustomTextureNodePrivate::buildTexture(const QSize& size)
{
    //creating output texture
    VkImageCreateInfo imageInfo;
    memset(&imageInfo, 0, sizeof(imageInfo));
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.flags = 0;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imageInfo.extent.width = uint32_t(size.width());
    imageInfo.extent.height = uint32_t(size.height());
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

    imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    imageInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VkImage image = VK_NULL_HANDLE;
    if (m_devFuncs->vkCreateImage(m_dev, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        qCritical("VulkanWrapper: failed to create image!");
        return  false;
    }

    m_texture = image;

    //creating texture memory
    VkMemoryRequirements memReq;
    m_devFuncs->vkGetImageMemoryRequirements(m_dev, image, &memReq);

    quint32 memIndex = 0;
    VkPhysicalDeviceMemoryProperties physDevMemProps;
    m_window->vulkanInstance()->functions()->vkGetPhysicalDeviceMemoryProperties(m_physDev, &physDevMemProps);
    for (uint32_t i = 0; i < physDevMemProps.memoryTypeCount; ++i) {
        if (!(memReq.memoryTypeBits & (1 << i)))
            continue;
        memIndex = i;
    }

    VkMemoryAllocateInfo allocInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        memReq.size,
        memIndex
    };

    VkResult err = m_devFuncs->vkAllocateMemory(m_dev, &allocInfo, nullptr, &m_textureMemory);
    if (err != VK_SUCCESS) {
        qWarning("Failed to allocate memory for linear image: %d", err);
        return false;
    }

    //bind memory to texture
    err = m_devFuncs->vkBindImageMemory(m_dev, image, m_textureMemory, 0);
    if (err != VK_SUCCESS) {
        qWarning("Failed to bind linear image memory: %d", err);
        return false;
    }

    //transiting layout from VK_IMAGE_LAYOUT_PREINITIALIZED to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    auto commandBuffer = BeginSingleTimeCommands();
    VkImageMemoryBarrier imageTransitionBarrier = {};
    imageTransitionBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageTransitionBarrier.srcAccessMask = VK_ACCESS_NONE;
    imageTransitionBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    imageTransitionBarrier.oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
    imageTransitionBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageTransitionBarrier.image = m_texture;
    imageTransitionBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageTransitionBarrier.subresourceRange.levelCount = imageTransitionBarrier.subresourceRange.layerCount = 1;

    m_devFuncs->vkCmdPipelineBarrier(commandBuffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0, 0, nullptr, 0, nullptr,
        1, &imageTransitionBarrier);
    EndSingleTimeCommands(commandBuffer);
    return true;
}

void CustomTextureNodePrivate::freeTexture()
{
    if (m_texture) {
        m_devFuncs->vkFreeMemory(m_dev, m_textureMemory, nullptr);
        m_textureMemory = VK_NULL_HANDLE;
        m_devFuncs->vkDestroyImage(m_dev, m_texture, nullptr);
        m_texture = VK_NULL_HANDLE;
    }
}

bool CustomTextureNodePrivate::initialize()
{
    const int framesInFlight = m_window->graphicsStateInfo().framesInFlight;
    m_initialized = true;

    QSGRendererInterface* rif = m_window->rendererInterface();
    QVulkanInstance* inst = reinterpret_cast<QVulkanInstance*>(
        rif->getResource(m_window, QSGRendererInterface::VulkanInstanceResource));
    Q_ASSERT(inst && inst->isValid());

    m_physDev = *static_cast<VkPhysicalDevice*>(rif->getResource(m_window, QSGRendererInterface::PhysicalDeviceResource));
    m_dev = *static_cast<VkDevice*>(rif->getResource(m_window, QSGRendererInterface::DeviceResource));
    Q_ASSERT(m_physDev && m_dev);

    //Initializing math renderer
    FVulkanStatic::SubscribeToContext(inst->vkInstance(), m_physDev);

    m_documentRendering.Init(AppGlobal::application->getFreeTypeWrap());

    m_devFuncs = inst->deviceFunctions(m_dev);
    m_funcs = inst->functions();

    //Find graphics queue family index
    uint32_t queueFamilyCount = 0;
    m_funcs->vkGetPhysicalDeviceQueueFamilyProperties(m_physDev, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    m_funcs->vkGetPhysicalDeviceQueueFamilyProperties(m_physDev, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            m_graphicsFamily = i;
            break;
        }
        i++;
    }

    //creating graphics command pool
    createCommandPool();
    //caching graphics queue
    m_devFuncs->vkGetDeviceQueue(m_dev, m_graphicsFamily, 0, &m_graphicsQueue);

    return true;
}

void CustomTextureNodePrivate::sync()
{
    m_dpr = m_window->effectiveDevicePixelRatio();
    QSize newSize = m_item->size().toSize() * m_dpr;
    newSize = QSize(qMax(newSize.width(), 1), qMax(newSize.height(), 1));
    bool needsNew = false;

    if (newSize != m_size) {
        //if window resized - update size
        needsNew = true;
        m_size = newSize;
        m_documentRendering.SetDocumentExtent({ uint32_t(m_size.width()), uint32_t(m_size.height()) });
        m_meDocState->Invalidate();
    }

    if (!m_initialized) {
        initialize();
        m_initialized = true;
    }

    if (!texture())
        needsNew = true;


    if (needsNew) {
        delete texture();
        freeTexture();
        buildTexture(m_size);
        QSGTexture* wrapper = QNativeInterface::QSGVulkanTexture::fromNative(m_texture,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            m_window,
            m_size);
        setTexture(wrapper);
        Q_ASSERT(wrapper->nativeInterface<QNativeInterface::QSGVulkanTexture>()->nativeImage() == m_texture);
    }
    //update math renderer state
    m_documentRendering.UpdateState(*m_meDocState);
    m_meDocState->Update();
}

void CustomTextureNodePrivate::setMeDocState(FMathDocumentState* meDocState)
{
    m_meDocState = meDocState;
}

void CustomTextureNodePrivate::render()
{
    if (!m_initialized)
        return;

    VkResult err = VK_SUCCESS;

    //render math document
    auto RenderedDocument = m_documentRendering.Render();
    auto RenderedDocBuffer = VkHelpers::ConvertImageToBuffer(RenderedDocument);
    void* RenderedDocData = RenderedDocBuffer->MapData();

    //Copy buffer into image
    VkDeviceSize imageSize = m_size.width() * m_size.height() * 4;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
    
    //move result into staging buffer
    void* data;
    m_devFuncs->vkMapMemory(m_dev, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, RenderedDocData, static_cast<size_t>(imageSize));
    RenderedDocBuffer->UnmapData();
    m_devFuncs->vkUnmapMemory(m_dev, stagingBufferMemory);

    //copy buffer into node texture
    TransitionImageLayout(m_texture, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(stagingBuffer, m_texture, static_cast<uint32_t>(m_size.width()), static_cast<uint32_t>(m_size.height()));
    TransitionImageLayout(m_texture, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    //destroy staging buffer
    m_devFuncs->vkDestroyBuffer(m_dev, stagingBuffer, nullptr);
    m_devFuncs->vkFreeMemory(m_dev, stagingBufferMemory, nullptr);
}

uint32_t CustomTextureNodePrivate::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    m_funcs->vkGetPhysicalDeviceMemoryProperties(m_physDev, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void CustomTextureNodePrivate::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (m_devFuncs->vkCreateBuffer(m_dev, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    m_devFuncs->vkGetBufferMemoryRequirements(m_dev, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    if (m_devFuncs->vkAllocateMemory(m_dev, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    m_devFuncs->vkBindBufferMemory(m_dev, buffer, bufferMemory, 0);
}

void CustomTextureNodePrivate::createCommandPool()
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = m_graphicsFamily;

    if (m_devFuncs->vkCreateCommandPool(m_dev, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics command pool!");
    }
}

VkCommandBuffer CustomTextureNodePrivate::BeginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    m_devFuncs->vkAllocateCommandBuffers(m_dev, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    m_devFuncs->vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void CustomTextureNodePrivate::EndSingleTimeCommands(VkCommandBuffer commandBuffer) {
    m_devFuncs->vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    m_devFuncs->vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    m_devFuncs->vkQueueWaitIdle(m_graphicsQueue);

    m_devFuncs->vkFreeCommandBuffers(m_dev, m_commandPool, 1, &commandBuffer);
}

void CustomTextureNodePrivate::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    auto CommandBuffer = BeginSingleTimeCommands();
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    m_devFuncs->vkCmdPipelineBarrier(
        CommandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );
    EndSingleTimeCommands(CommandBuffer);
}

void CustomTextureNodePrivate::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    auto CommandBuffer = BeginSingleTimeCommands();
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
    width,
    height,
    1
    };

    m_devFuncs->vkCmdCopyBufferToImage(CommandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    EndSingleTimeCommands(CommandBuffer);
}

#include "MathDocument.moc"

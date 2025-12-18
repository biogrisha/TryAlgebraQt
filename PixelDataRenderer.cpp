#include "PixelDataRenderer.h"
#include <QtCore/QRunnable>
#include <QtQuick/QQuickWindow>

#include <QVulkanInstance>
#include <QVulkanFunctions>
#include <QFile>

#include <VulkanHelpers.h>
#include <VertexInputLayout.h>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <VulkanContext.h>
#include <MathDocumentRendering.h>
#include <VulkanHelpers.h>
#include <QElapsedTimer>

struct FVertex
{
    glm::vec2 Pos;
    glm::vec2 Uv;
};

class FVertexLayoutPresent
{
public:
    std::vector<VkVertexInputBindingDescription> getBindingDescription()
    {
        return {
            { 0, sizeof(FVertex), VK_VERTEX_INPUT_RATE_VERTEX }
        };
    }
    virtual std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
    {
        return {
            VkVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(FVertex, Pos)),
            VkVertexInputAttributeDescription(1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(FVertex, Uv)),
        };
    }
};

class VulkanRendererPrivate : public QObject
{
    Q_OBJECT
public:
    ~VulkanRendererPrivate();

    void setT(qreal t) { m_t = t; }
    void setViewportSize(const QSize& size) { m_viewportSize = size; }
    void setWindow(QQuickWindow* window) { m_window = window; }

public slots:
    void frameStart();
    void mainPassRecordingStart();

private:
    enum Stage {
        VertexStage,
        FragmentStage
    };
    void prepareShader();
    void init(int framesInFlight);
    void MyInit(int framesInFlight);
    void createCommandPool();
    VkCommandBuffer BeginSingleTimeCommands();
    void EndSingleTimeCommands(VkCommandBuffer CommandBuffer);
    void CreateTextureImage();
    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void CreatePipeline(VkRenderPass rp);
    void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    QSize m_viewportSize;
    qreal m_t = 0;
    QQuickWindow* m_window = nullptr;

    QByteArray m_shader;

    bool m_initialized = false;
    VkPhysicalDevice m_physDev = VK_NULL_HANDLE;
    VkDevice m_dev = VK_NULL_HANDLE;
    QVulkanDeviceFunctions* m_devFuncs = nullptr;
    QVulkanFunctions* m_funcs = nullptr;

    VkBuffer m_vbuf = VK_NULL_HANDLE;
    VkDeviceMemory m_vbufMem = VK_NULL_HANDLE;

    VkPipelineCache m_pipelineCache = VK_NULL_HANDLE;

    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_resLayout = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;

    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    VkDescriptorSet m_texDescriptor = VK_NULL_HANDLE;

    VkShaderModule ShaderModule = VK_NULL_HANDLE;

    VkImage textureImage = VK_NULL_HANDLE;
    VkDeviceMemory textureImageMemory = VK_NULL_HANDLE;
    VkImageView textureImageView = VK_NULL_HANDLE;
    VkSampler textureSampler = VK_NULL_HANDLE;
    FVertexLayoutPresent VertexLayoutPresent;

    uint32_t graphicsFamily = 0;

    VkCommandPool commandPool;
    VkQueue graphicsQueue;

    struct FContextDestroyer
    {
        ~FContextDestroyer()
        {
            FVulkanStatic::Context.reset();
        }
    } ContextDestroyer;

    FMathDocumentRendering DocumentRendering;
};

PixelDataRenderer::PixelDataRenderer()
{
    //when new window is assigned as a parent to this QQuickItem
    connect(this, &QQuickItem::windowChanged, this, &PixelDataRenderer::handleWindowChanged);
}

void PixelDataRenderer::setT(qreal t)
{
    if (t == m_t)
        return;
    m_t = t;
    emit tChanged();
    if (window())
        window()->update();
}

void PixelDataRenderer::handleWindowChanged(QQuickWindow* win)
{
    if (win) {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &PixelDataRenderer::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &PixelDataRenderer::cleanup, Qt::DirectConnection);

        // Ensure we start with cleared to black. The squircle's blend mode relies on this.
        win->setColor(Qt::black);
    }
}

// The safe way to release custom graphics resources is to both connect to
// sceneGraphInvalidated() and implement releaseResources(). To support
// threaded render loops the latter performs the SquircleRenderer destruction
// via scheduleRenderJob(). Note that the VulkanSquircle may be gone by the time
// the QRunnable is invoked.

void PixelDataRenderer::cleanup()
{
    delete m_renderer;
    m_renderer = nullptr;
}

class CleanupJob : public QRunnable
{
public:
    CleanupJob(VulkanRendererPrivate* renderer) : m_renderer(renderer) {}
    void run() override { delete m_renderer; }
private:
    VulkanRendererPrivate* m_renderer;
};

void PixelDataRenderer::releaseResources()
{
    //schedule private renderer deletion to be executed at BeforeSynchronizingStage
    window()->scheduleRenderJob(new CleanupJob(m_renderer), QQuickWindow::BeforeSynchronizingStage);
    m_renderer = nullptr;
}

VulkanRendererPrivate::~VulkanRendererPrivate()
{
    qDebug("cleanup");
    if (!m_devFuncs)
        return;

    m_devFuncs->vkDestroyCommandPool(m_dev, commandPool, nullptr);
    m_devFuncs->vkDestroyPipeline(m_dev, m_pipeline, nullptr);
    m_devFuncs->vkDestroyPipelineLayout(m_dev, m_pipelineLayout, nullptr);
    m_devFuncs->vkDestroyDescriptorSetLayout(m_dev, m_resLayout, nullptr);

    m_devFuncs->vkDestroyDescriptorPool(m_dev, m_descriptorPool, nullptr);

    m_devFuncs->vkDestroyPipelineCache(m_dev, m_pipelineCache, nullptr);

    m_devFuncs->vkDestroyBuffer(m_dev, m_vbuf, nullptr);
    m_devFuncs->vkFreeMemory(m_dev, m_vbufMem, nullptr);

    m_devFuncs->vkDestroySampler(m_dev, textureSampler, nullptr);
    m_devFuncs->vkDestroyImageView(m_dev, textureImageView, nullptr);

    m_devFuncs->vkDestroyImage(m_dev, textureImage, nullptr);
    m_devFuncs->vkFreeMemory(m_dev, textureImageMemory, nullptr);

    qDebug("released");
}

void PixelDataRenderer::sync()
{
    if (!m_renderer) {
        m_renderer = new VulkanRendererPrivate;
        // Initializing resources is done before starting to record the
        // renderpass, regardless of wanting an underlay or overlay.
        connect(window(), &QQuickWindow::beforeRendering, m_renderer, &VulkanRendererPrivate::frameStart, Qt::DirectConnection);
        // Here we want an underlay and therefore connect to
        // beforeRenderPassRecording. Changing to afterRenderPassRecording
        // would render the squircle on top (overlay).
        connect(window(), &QQuickWindow::beforeRenderPassRecording, m_renderer, &VulkanRendererPrivate::mainPassRecordingStart, Qt::DirectConnection);
    }
    m_renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
    m_renderer->setT(m_t);
    m_renderer->setWindow(window());
}

void VulkanRendererPrivate::frameStart()
{
    QSGRendererInterface* rif = m_window->rendererInterface();

    // We are not prepared for anything other than running with the RHI and its Vulkan backend.
    Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::Vulkan);

    if (!m_initialized)
        init(m_window->graphicsStateInfo().framesInFlight);
}

std::vector<FVertex> vertices =
{
    {{-1, -1,}, {0.0f, 0.0f}},
    {{1, -1,},  {1.0f, 0.0f}},
    {{-1, 1,},  {0.0f, 1.0f}},
    {{1, 1},    {1.0f, 1.0f}}
};

const int UBUF_SIZE = 4;

std::wstring Chars = L"abasfdadsfcvjhjhvjlhvhgergergergwergwergwrasdfasdfasdfasdfasdfasdfasdfasdfasdfasdfegwerg";
int Ind = 21;
int Size = 5;

void VulkanRendererPrivate::mainPassRecordingStart()
{
    // This example demonstrates the simple case: prepending some commands to
    // the scenegraph's main renderpass. It does not create its own passes,
    // rendertargets, etc. so no synchronization is needed.

    const QQuickWindow::GraphicsStateInfo& stateInfo(m_window->graphicsStateInfo());
    QSGRendererInterface* rif = m_window->rendererInterface();

    m_window->beginExternalCommands();

    // Must query the command buffer _after_ beginExternalCommands(), this is
    // actually important when running on Vulkan because what we get here is a
    // new secondary command buffer, not the primary one.
    VkCommandBuffer cb = *reinterpret_cast<VkCommandBuffer*>(
        rif->getResource(m_window, QSGRendererInterface::CommandListResource));
    Q_ASSERT(cb);

    //Draw math document
    std::vector<FGlyphData> Glyphs;
    for (int i = 0; i < Size; i++)
    {
        int OffsetY = 0;
        for (int j = Ind - 20; j < Ind; j++)
        {

            FGlyphData g;
            g.GlyphId.Glyph = Chars[j];
            g.GlyphId.Height = Size;
            g.Pos = glm::vec2(i * 30, OffsetY);
            Glyphs.push_back(g);

            OffsetY += 40;
        }
    }
    Ind = std::min<int>(Chars.size(), Ind + 1);
    Size = std::min<int>(50, Size + 1);
    DocumentRendering.SetDocumentContent(Glyphs);
    auto RenderedDocument = DocumentRendering.Render();
    auto RenderedDocBuffer = VkHelpers::ConvertImageToBuffer(RenderedDocument);
    void* RenderedDocData = RenderedDocBuffer->MapData();

    //Copy buffer into image
    VkDeviceSize imageSize = 1000 * 1000 * 4;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    std::vector<char> pixels(1000 * 1000 * 4, 0);
    memcpy(pixels.data(), RenderedDocData, static_cast<size_t>(imageSize));

    void* data;
    m_devFuncs->vkMapMemory(m_dev, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, RenderedDocData, static_cast<size_t>(imageSize));
    RenderedDocBuffer->UnmapData();
    m_devFuncs->vkUnmapMemory(m_dev, stagingBufferMemory);
    TransitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(1000), static_cast<uint32_t>(1000));
    TransitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    m_devFuncs->vkDestroyBuffer(m_dev, stagingBuffer, nullptr);
    m_devFuncs->vkFreeMemory(m_dev, stagingBufferMemory, nullptr);
    // Do not assume any state persists on the command buffer. (it may be a
    // brand new one that just started recording)

    m_devFuncs->vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

    VkDeviceSize vbufOffset = 0;
    m_devFuncs->vkCmdBindVertexBuffers(cb, 0, 1, &m_vbuf, &vbufOffset);

    uint32_t Offset = 0;
    m_devFuncs->vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1,
        &m_texDescriptor, 0, &Offset);

    VkViewport vp = { 0, 0, float(m_viewportSize.width()), float(m_viewportSize.height()), 0.0f, 1.0f };
    m_devFuncs->vkCmdSetViewport(cb, 0, 1, &vp);
    VkRect2D scissor = { { 0, 0 }, { uint32_t(m_viewportSize.width()), uint32_t(m_viewportSize.height()) } };
    m_devFuncs->vkCmdSetScissor(cb, 0, 1, &scissor);

    m_devFuncs->vkCmdDraw(cb, 4, 1, 0, 0);

    m_window->endExternalCommands();
}

void VulkanRendererPrivate::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
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

void VulkanRendererPrivate::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
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

void VulkanRendererPrivate::prepareShader()
{
    QString filename = QLatin1String(":/tryAlgebra/ThirdParty/Shader/DrawEditorLayout.spv");
    
    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly))
        qFatal("Failed to read shader %s", qPrintable(filename));

    const QByteArray contents = f.readAll();

    m_shader = contents;
    Q_ASSERT(!m_shader.isEmpty());
}

static inline VkDeviceSize aligned(VkDeviceSize v, VkDeviceSize byteAlign)
{
    return (v + byteAlign - 1) & ~(byteAlign - 1);
}

void VulkanRendererPrivate::init(int framesInFlight)
{
    Q_ASSERT(framesInFlight <= 3);
    m_initialized = true;

    QSGRendererInterface* rif = m_window->rendererInterface();
    QVulkanInstance* inst = reinterpret_cast<QVulkanInstance*>(
        rif->getResource(m_window, QSGRendererInterface::VulkanInstanceResource));
    Q_ASSERT(inst && inst->isValid());

    m_physDev = *reinterpret_cast<VkPhysicalDevice*>(rif->getResource(m_window, QSGRendererInterface::PhysicalDeviceResource));
    m_dev = *reinterpret_cast<VkDevice*>(rif->getResource(m_window, QSGRendererInterface::DeviceResource));
    Q_ASSERT(m_physDev && m_dev);

    m_devFuncs = inst->deviceFunctions(m_dev);
    m_funcs = inst->functions();
    Q_ASSERT(m_devFuncs && m_funcs);

    VkRenderPass rp = *reinterpret_cast<VkRenderPass*>(
        rif->getResource(m_window, QSGRendererInterface::RenderPassResource));
    Q_ASSERT(rp);

    // For simplicity we just use host visible buffers instead of device local + staging.

    VkPhysicalDeviceProperties physDevProps;
    m_funcs->vkGetPhysicalDeviceProperties(m_physDev, &physDevProps);

    VkPhysicalDeviceMemoryProperties physDevMemProps;
    m_funcs->vkGetPhysicalDeviceMemoryProperties(m_physDev, &physDevMemProps);

    MyInit(framesInFlight);
    CreatePipeline(rp);
    CreateTextureImage();

    // Now just need some descriptors.
    VkDescriptorPoolSize descPoolSizes[] = {
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 }
    };
    VkDescriptorPoolCreateInfo descPoolInfo;
    memset(&descPoolInfo, 0, sizeof(descPoolInfo));
    descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descPoolInfo.flags = 0; // won't use vkFreeDescriptorSets
    descPoolInfo.maxSets = 1;
    descPoolInfo.poolSizeCount = sizeof(descPoolSizes) / sizeof(descPoolSizes[0]);
    descPoolInfo.pPoolSizes = descPoolSizes;
    auto err = m_devFuncs->vkCreateDescriptorPool(m_dev, &descPoolInfo, nullptr, &m_descriptorPool);
    if (err != VK_SUCCESS)
        qFatal("Failed to create descriptor pool: %d", err);

    VkDescriptorSetAllocateInfo descAllocInfo;
    memset(&descAllocInfo, 0, sizeof(descAllocInfo));
    descAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descAllocInfo.descriptorPool = m_descriptorPool;
    descAllocInfo.descriptorSetCount = 1;
    descAllocInfo.pSetLayouts = &m_resLayout;
    err = m_devFuncs->vkAllocateDescriptorSets(m_dev, &descAllocInfo, &m_texDescriptor);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate descriptor set");

    VkWriteDescriptorSet writeInfo;
    memset(&writeInfo, 0, sizeof(writeInfo));
    writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeInfo.dstSet = m_texDescriptor;
    writeInfo.dstBinding = 0;
    writeInfo.descriptorCount = 1;
    writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    VkDescriptorImageInfo imageInfo;
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = textureImageView;
    imageInfo.sampler = textureSampler;
    writeInfo.pImageInfo = &imageInfo;
    m_devFuncs->vkUpdateDescriptorSets(m_dev, 1, &writeInfo, 0, nullptr);

    //Find queue family index
    uint32_t queueFamilyCount = 0;
    m_funcs->vkGetPhysicalDeviceQueueFamilyProperties(m_physDev, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    m_funcs->vkGetPhysicalDeviceQueueFamilyProperties(m_physDev, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
        {
            graphicsFamily = i;
            break;
        }
        i++;
    }

    createCommandPool();
    m_devFuncs->vkGetDeviceQueue(m_dev, graphicsFamily, 0, &graphicsQueue);

    //Init math renderer
    FVulkanStatic::Context = std::make_unique<FVulkanContext>();
    FVulkanStatic::Context->Init(inst->vkInstance(), m_physDev);
    DocumentRendering.SetDocumentExtent({ 1000 , 1000 });
    DocumentRendering.Init();
}

void VulkanRendererPrivate::MyInit(int framesInFlight)
{
    VkPhysicalDeviceMemoryProperties physDevMemProps;
    m_funcs->vkGetPhysicalDeviceMemoryProperties(m_physDev, &physDevMemProps);

    //init vertex buffer
    VkBufferCreateInfo bufferInfo;
    memset(&bufferInfo, 0, sizeof(bufferInfo));
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(FVertex) * vertices.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    VkResult err = m_devFuncs->vkCreateBuffer(m_dev, &bufferInfo, nullptr, &m_vbuf);
    if (err != VK_SUCCESS)
        qFatal("Failed to create vertex buffer: %d", err);

    VkMemoryRequirements memReq;
    m_devFuncs->vkGetBufferMemoryRequirements(m_dev, m_vbuf, &memReq);
    VkMemoryAllocateInfo allocInfo;
    memset(&allocInfo, 0, sizeof(allocInfo));
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;

    uint32_t memTypeIndex = uint32_t(-1);
    const VkMemoryType* memType = physDevMemProps.memoryTypes;
    for (uint32_t i = 0; i < physDevMemProps.memoryTypeCount; ++i) {
        if (memReq.memoryTypeBits & (1 << i)) {
            if ((memType[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
                && (memType[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
            {
                memTypeIndex = i;
                break;
            }
        }
    }
    if (memTypeIndex == uint32_t(-1))
        qFatal("Failed to find host visible and coherent memory type");

    allocInfo.memoryTypeIndex = memTypeIndex;
    err = m_devFuncs->vkAllocateMemory(m_dev, &allocInfo, nullptr, &m_vbufMem);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate vertex buffer memory of size %u: %d", uint(allocInfo.allocationSize), err);

    void* p = nullptr;
    err = m_devFuncs->vkMapMemory(m_dev, m_vbufMem, 0, allocInfo.allocationSize, 0, &p);
    if (err != VK_SUCCESS || !p)
        qFatal("Failed to map vertex buffer memory: %d", err);
    memcpy(p, vertices.data(), sizeof(FVertex) * vertices.size());
    m_devFuncs->vkUnmapMemory(m_dev, m_vbufMem);
    err = m_devFuncs->vkBindBufferMemory(m_dev, m_vbuf, m_vbufMem, 0);
    if (err != VK_SUCCESS)
        qFatal("Failed to bind vertex buffer memory: %d", err);
}

void VulkanRendererPrivate::createCommandPool()
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = graphicsFamily;

    if (m_devFuncs->vkCreateCommandPool(m_dev, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to create graphics command pool!");
    }
}

VkCommandBuffer VulkanRendererPrivate::BeginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    m_devFuncs->vkAllocateCommandBuffers(m_dev, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    m_devFuncs->vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void VulkanRendererPrivate::EndSingleTimeCommands(VkCommandBuffer commandBuffer) {
    m_devFuncs->vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    m_devFuncs->vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    m_devFuncs->vkQueueWaitIdle(graphicsQueue);

    m_devFuncs->vkFreeCommandBuffers(m_dev, commandPool, 1, &commandBuffer);
}

void VulkanRendererPrivate::CreateTextureImage() {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = static_cast<uint32_t>(1000);
    imageInfo.extent.height = static_cast<uint32_t>(1000);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0; // Optional
    if (m_devFuncs->vkCreateImage(m_dev, &imageInfo, nullptr, &textureImage) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    m_devFuncs->vkGetImageMemoryRequirements(m_dev, textureImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (m_devFuncs->vkAllocateMemory(m_dev, &allocInfo, nullptr, &textureImageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    m_devFuncs->vkBindImageMemory(m_dev, textureImage, textureImageMemory, 0);


    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = textureImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (m_devFuncs->vkCreateImageView(m_dev, &viewInfo, nullptr, &textureImageView) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to create texture image view!");
    }

    VkPhysicalDeviceProperties properties{};
    m_funcs->vkGetPhysicalDeviceProperties(m_physDev, &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    if (m_devFuncs->vkCreateSampler(m_dev, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

void VulkanRendererPrivate::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
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

uint32_t VulkanRendererPrivate::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    m_funcs->vkGetPhysicalDeviceMemoryProperties(m_physDev, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void VulkanRendererPrivate::CreatePipeline(VkRenderPass rp)
{
    //Pipeline cache info
    VkPipelineCacheCreateInfo pipelineCacheInfo;
    memset(&pipelineCacheInfo, 0, sizeof(pipelineCacheInfo));
    pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    auto err = m_devFuncs->vkCreatePipelineCache(m_dev, &pipelineCacheInfo, nullptr, &m_pipelineCache);
    if (err != VK_SUCCESS)
        qFatal("Failed to create pipeline cache: %d", err);

    //Descriptor set layout binding
    VkDescriptorSetLayoutBinding descLayoutBinding;
    memset(&descLayoutBinding, 0, sizeof(descLayoutBinding));
    descLayoutBinding.binding = 0;
    descLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descLayoutBinding.descriptorCount = 1;
    descLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    
    //Descriptor set layout
    VkDescriptorSetLayoutCreateInfo layoutInfo;
    memset(&layoutInfo, 0, sizeof(layoutInfo));
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &descLayoutBinding;
    err = m_devFuncs->vkCreateDescriptorSetLayout(m_dev, &layoutInfo, nullptr, &m_resLayout);
    if (err != VK_SUCCESS)
        qFatal("Failed to create descriptor set layout: %d", err);
    
    //Pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo;
    memset(&pipelineLayoutInfo, 0, sizeof(pipelineLayoutInfo));
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_resLayout;
    err = m_devFuncs->vkCreatePipelineLayout(m_dev, &pipelineLayoutInfo, nullptr, &m_pipelineLayout);
    if (err != VK_SUCCESS)
        qWarning("Failed to create pipeline layout: %d", err);


    prepareShader();
    //create shaders for pipeline
    ShaderModule = NVkHelpers::createShaderModule(reinterpret_cast<const uint32_t*>(m_shader.constData()), m_shader.size(), m_dev);
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = ShaderModule;
    vertShaderStageInfo.pName = "vertMain";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = ShaderModule;
    fragShaderStageInfo.pName = "fragMain";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };


    auto BindingDescriptions = VertexLayoutPresent.getBindingDescription();
    auto VertexAttributeDescriptions = VertexLayoutPresent.getAttributeDescriptions();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    memset(&vertexInputInfo, 0, sizeof(vertexInputInfo));
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = BindingDescriptions.size();
    vertexInputInfo.pVertexBindingDescriptions = BindingDescriptions.data();
    vertexInputInfo.vertexAttributeDescriptionCount = VertexAttributeDescriptions.size();
    vertexInputInfo.pVertexAttributeDescriptions = VertexAttributeDescriptions.data();

    VkDynamicState dynStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicInfo;
    memset(&dynamicInfo, 0, sizeof(dynamicInfo));
    dynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicInfo.dynamicStateCount = 2;
    dynamicInfo.pDynamicStates = dynStates;

    VkPipelineViewportStateCreateInfo viewportInfo;
    memset(&viewportInfo, 0, sizeof(viewportInfo));
    viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.viewportCount = viewportInfo.scissorCount = 1;

    VkPipelineInputAssemblyStateCreateInfo iaInfo;
    memset(&iaInfo, 0, sizeof(iaInfo));
    iaInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    iaInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

    VkPipelineRasterizationStateCreateInfo rsInfo;
    memset(&rsInfo, 0, sizeof(rsInfo));
    rsInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rsInfo.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo msInfo;
    memset(&msInfo, 0, sizeof(msInfo));
    msInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    msInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo dsInfo;
    memset(&dsInfo, 0, sizeof(dsInfo));
    dsInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    // SrcAlpha, One
    VkPipelineColorBlendStateCreateInfo blendInfo;
    memset(&blendInfo, 0, sizeof(blendInfo));
    blendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    VkPipelineColorBlendAttachmentState blend;
    memset(&blend, 0, sizeof(blend));
    blend.blendEnable = true;
    blend.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blend.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
    blend.colorBlendOp = VK_BLEND_OP_ADD;
    blend.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    blend.alphaBlendOp = VK_BLEND_OP_ADD;
    blend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
        | VK_COLOR_COMPONENT_A_BIT;
    blendInfo.attachmentCount = 1;
    blendInfo.pAttachments = &blend;

    //Pipeline create info
    VkGraphicsPipelineCreateInfo pipelineInfo;
    memset(&pipelineInfo, 0, sizeof(pipelineInfo));
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pDynamicState = &dynamicInfo;
    pipelineInfo.pViewportState = &viewportInfo;
    pipelineInfo.pInputAssemblyState = &iaInfo;
    pipelineInfo.pRasterizationState = &rsInfo;
    pipelineInfo.pMultisampleState = &msInfo;
    pipelineInfo.pDepthStencilState = &dsInfo;
    pipelineInfo.pColorBlendState = &blendInfo;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = rp;

    err = m_devFuncs->vkCreateGraphicsPipelines(m_dev, m_pipelineCache, 1, &pipelineInfo, nullptr, &m_pipeline);
    m_devFuncs->vkDestroyShaderModule(m_dev, ShaderModule, nullptr);
}

#include "PixelDataRenderer.moc"


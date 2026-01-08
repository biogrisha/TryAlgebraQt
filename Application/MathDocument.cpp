// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "MathDocument.h"

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
#include <Application.h>
#include <FreeTypeWrap.h>
#include <AppGlobal.h>

//Screen rectangle vertices
struct Vertex
{
    glm::vec2 Pos;
    glm::vec2 Uv;
};

//Pipeline vertex layout to draw rectangle with texture
class VertexLayoutPresent
{
public:
    std::vector<VkVertexInputBindingDescription> getBindingDescription()
    {
        return {
            { 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX }
        };
    }
    virtual std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
    {
        return {
            VkVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, Pos)),
            VkVertexInputAttributeDescription(1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, Uv)),
        };
    }
};

class CustomTextureNodePrivate : public QSGTextureProvider, public QSGSimpleTextureNode
{
    Q_OBJECT

public:
    CustomTextureNodePrivate(QQuickItem* item);
    ~CustomTextureNodePrivate() override;

    QSGTexture* texture() const override;

    void sync();
    void setMeDocState(FMathDocumentState* meDocState);
private slots:
    void render();

private:
    enum Stage {
        VertexStage,
        FragmentStage
    };
    bool buildTexture(const QSize& size);
    void freeTexture();
    bool createRenderPass();
    bool initialize();
    void CreateTextureImage();
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void prepareShader();
    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void createCommandPool();
    VkCommandBuffer BeginSingleTimeCommands();
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
    void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    QQuickItem* m_item;
    QQuickWindow* m_window;
    QSize m_size;
    float m_itemSizeX = 0;
    float m_itemSizeY = 0;
    qreal m_dpr;

    QByteArray m_shader;

    VkImage m_texture = VK_NULL_HANDLE;
    VkDeviceMemory m_textureMemory = VK_NULL_HANDLE;
    VkFramebuffer m_textureFramebuffer = VK_NULL_HANDLE;
    VkImageView m_textureView = VK_NULL_HANDLE;

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

    VkRenderPass m_renderPass = VK_NULL_HANDLE;

    struct ContextDestroyer
    {
        ~ContextDestroyer()
        {
            FVulkanStatic::UnsubscribeFromContext();
        }
    } m_contextDestroyer;


    FMathDocumentRendering m_documentRendering;

    VkImage m_textureImage = VK_NULL_HANDLE;
    VkDeviceMemory m_textureImageMemory = VK_NULL_HANDLE;
    VkImageView m_textureImageView = VK_NULL_HANDLE;
    VkSampler m_textureSampler = VK_NULL_HANDLE;
    VertexLayoutPresent m_vertexLayoutPresent;

    VkShaderModule m_shaderModule = VK_NULL_HANDLE;
    uint32_t m_graphicsFamily = 0;
    VkCommandPool m_commandPool;
    VkQueue m_graphicsQueue;
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
    m_devFuncs->vkDestroyBuffer(m_dev, m_vbuf, nullptr);
    m_devFuncs->vkFreeMemory(m_dev, m_vbufMem, nullptr);

    m_devFuncs->vkDestroyPipelineCache(m_dev, m_pipelineCache, nullptr);
    m_devFuncs->vkDestroyPipelineLayout(m_dev, m_pipelineLayout, nullptr);
    m_devFuncs->vkDestroyPipeline(m_dev, m_pipeline, nullptr);

    m_devFuncs->vkDestroyRenderPass(m_dev, m_renderPass, nullptr);

    m_devFuncs->vkDestroyDescriptorSetLayout(m_dev, m_resLayout, nullptr);
    m_devFuncs->vkDestroyDescriptorPool(m_dev, m_descriptorPool, nullptr);

    m_devFuncs->vkDestroySampler(m_dev, m_textureSampler, nullptr);
    m_devFuncs->vkDestroyImageView(m_dev, m_textureImageView, nullptr);

    m_devFuncs->vkDestroyImage(m_dev, m_textureImage, nullptr);
    m_devFuncs->vkFreeMemory(m_dev, m_textureImageMemory, nullptr);

    delete texture();
    freeTexture();
}

QSGTexture* CustomTextureNodePrivate::texture() const
{
    return QSGSimpleTextureNode::texture();
}

std::vector<Vertex> rectVertices =
{
    {{-1, -1,}, {0.0f, 0.0f}},
    {{1, -1,},  {1.0f, 0.0f}},
    {{-1, 1,},  {0.0f, 1.0f}},
    {{1, 1},    {1.0f, 1.0f}}
};

bool CustomTextureNodePrivate::buildTexture(const QSize& size)
{
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

    err = m_devFuncs->vkBindImageMemory(m_dev, image, m_textureMemory, 0);
    if (err != VK_SUCCESS) {
        qWarning("Failed to bind linear image memory: %d", err);
        return false;
    }

    VkImageViewCreateInfo viewInfo;
    memset(&viewInfo, 0, sizeof(viewInfo));
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = imageInfo.format;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

    err = m_devFuncs->vkCreateImageView(m_dev, &viewInfo, nullptr, &m_textureView);
    if (err != VK_SUCCESS) {
        qWarning("Failed to create render target image view: %d", err);
        return false;
    }

    VkFramebufferCreateInfo fbInfo;
    memset(&fbInfo, 0, sizeof(fbInfo));
    fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbInfo.renderPass = m_renderPass;
    fbInfo.attachmentCount = 1;
    fbInfo.pAttachments = &m_textureView;
    fbInfo.width = uint32_t(size.width());
    fbInfo.height = uint32_t(size.height());
    fbInfo.layers = 1;

    err = m_devFuncs->vkCreateFramebuffer(m_dev, &fbInfo, nullptr, &m_textureFramebuffer);
    if (err != VK_SUCCESS) {
        qWarning("Failed to create framebuffer: %d", err);
        return false;
    }
    return true;
}

void CustomTextureNodePrivate::freeTexture()
{
    if (m_texture) {
        m_devFuncs->vkDestroyFramebuffer(m_dev, m_textureFramebuffer, nullptr);
        m_textureFramebuffer = VK_NULL_HANDLE;
        m_devFuncs->vkFreeMemory(m_dev, m_textureMemory, nullptr);
        m_textureMemory = VK_NULL_HANDLE;
        m_devFuncs->vkDestroyImageView(m_dev, m_textureView, nullptr);
        m_textureView = VK_NULL_HANDLE;
        m_devFuncs->vkDestroyImage(m_dev, m_texture, nullptr);
        m_texture = VK_NULL_HANDLE;
    }
}



static inline VkDeviceSize aligned(VkDeviceSize v, VkDeviceSize byteAlign)
{
    return (v + byteAlign - 1) & ~(byteAlign - 1);
}

bool CustomTextureNodePrivate::createRenderPass()
{
    const VkFormat vkformat = VK_FORMAT_R8G8B8A8_UNORM;
    const VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
    VkAttachmentDescription colorAttDesc;
    memset(&colorAttDesc, 0, sizeof(colorAttDesc));
    colorAttDesc.format = vkformat;
    colorAttDesc.samples = samples;
    colorAttDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    const VkAttachmentReference colorRef = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

    VkSubpassDescription subpassDesc;
    memset(&subpassDesc, 0, sizeof(subpassDesc));
    subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDesc.colorAttachmentCount = 1;
    subpassDesc.pColorAttachments = &colorRef;
    subpassDesc.pDepthStencilAttachment = nullptr;
    subpassDesc.pResolveAttachments = nullptr;

    VkRenderPassCreateInfo rpInfo;
    memset(&rpInfo, 0, sizeof(rpInfo));
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rpInfo.attachmentCount = 1;
    rpInfo.pAttachments = &colorAttDesc;
    rpInfo.subpassCount = 1;
    rpInfo.pSubpasses = &subpassDesc;

    VkResult err = m_devFuncs->vkCreateRenderPass(m_dev, &rpInfo, nullptr, &m_renderPass);
    if (err != VK_SUCCESS) {
        qWarning("Failed to create renderpass: %d", err);
        return false;
    }

    return true;
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
    Q_ASSERT(m_devFuncs && m_funcs);

    createRenderPass();

    VkPhysicalDeviceProperties physDevProps;
    m_funcs->vkGetPhysicalDeviceProperties(m_physDev, &physDevProps);

    VkPhysicalDeviceMemoryProperties physDevMemProps;
    m_funcs->vkGetPhysicalDeviceMemoryProperties(m_physDev, &physDevMemProps);

    //initializing vertex buffer
    auto AllocationSize = sizeof(Vertex) * rectVertices.size();
    CreateBuffer(sizeof(Vertex) * rectVertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_vbuf, m_vbufMem);
    void* P = nullptr;
    auto Err = m_devFuncs->vkMapMemory(m_dev, m_vbufMem, 0, AllocationSize, 0, &P);
    if (Err != VK_SUCCESS || !P)
    {
        qFatal("Failed to map vertex buffer memory: %d", Err);
    }
    memcpy(P, rectVertices.data(), sizeof(Vertex) * rectVertices.size());
    m_devFuncs->vkUnmapMemory(m_dev, m_vbufMem);
    if (Err != VK_SUCCESS)
    {
        qFatal("Failed to bind vertex buffer memory: %d", Err);
    }

    CreateTextureImage();

    // Now onto the pipeline.

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
    m_shaderModule = NVkHelpers::createShaderModule(reinterpret_cast<const uint32_t*>(m_shader.constData()), m_shader.size(), m_dev);
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = m_shaderModule;
    vertShaderStageInfo.pName = "vertMain";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = m_shaderModule;
    fragShaderStageInfo.pName = "fragMain";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    
    auto BindingDescriptions = m_vertexLayoutPresent.getBindingDescription();
    auto VertexAttributeDescriptions = m_vertexLayoutPresent.getAttributeDescriptions();
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
    pipelineInfo.renderPass = m_renderPass;

    err = m_devFuncs->vkCreateGraphicsPipelines(m_dev, m_pipelineCache, 1, &pipelineInfo, nullptr, &m_pipeline);

    m_devFuncs->vkDestroyShaderModule(m_dev, m_shaderModule, nullptr);

    if (err != VK_SUCCESS)
        qFatal("Failed to create graphics pipeline: %d", err);

    //creating descriptor pool
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

    err = m_devFuncs->vkCreateDescriptorPool(m_dev, &descPoolInfo, nullptr, &m_descriptorPool);
    if (err != VK_SUCCESS)
        qFatal("Failed to create descriptor pool: %d", err);

    //creating descriptor set
    VkDescriptorSetAllocateInfo descAllocInfo;
    memset(&descAllocInfo, 0, sizeof(descAllocInfo));
    descAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descAllocInfo.descriptorPool = m_descriptorPool;
    descAllocInfo.descriptorSetCount = 1;
    descAllocInfo.pSetLayouts = &m_resLayout;
    err = m_devFuncs->vkAllocateDescriptorSets(m_dev, &descAllocInfo, &m_texDescriptor);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate descriptor set");

    //binding texture to descriptor set
    VkWriteDescriptorSet writeInfo;
    memset(&writeInfo, 0, sizeof(writeInfo));
    writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeInfo.dstSet = m_texDescriptor;
    writeInfo.dstBinding = 0;
    writeInfo.descriptorCount = 1;
    writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    VkDescriptorImageInfo imageInfo;
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = m_textureImageView;
    imageInfo.sampler = m_textureSampler;
    writeInfo.pImageInfo = &imageInfo;
    m_devFuncs->vkUpdateDescriptorSets(m_dev, 1, &writeInfo, 0, nullptr);

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
    const QSize newSize = m_item->size().toSize() * m_dpr;
    bool needsNew = false;

    if (newSize != m_size) {
        needsNew = true;
        m_size = newSize;
        auto size = m_item->size().toSize();
        m_itemSizeY = size.height();
        m_itemSizeX = size.width();

        m_documentRendering.SetDocumentExtent({ uint32_t(m_itemSizeX), uint32_t(m_itemSizeY) });
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
    VkDeviceSize imageSize = m_itemSizeX * m_itemSizeY * 4;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    m_devFuncs->vkMapMemory(m_dev, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, RenderedDocData, static_cast<size_t>(imageSize));
    RenderedDocBuffer->UnmapData();
    m_devFuncs->vkUnmapMemory(m_dev, stagingBufferMemory);
    TransitionImageLayout(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(stagingBuffer, m_textureImage, static_cast<uint32_t>(m_itemSizeX), static_cast<uint32_t>(m_itemSizeY));
    TransitionImageLayout(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    m_devFuncs->vkDestroyBuffer(m_dev, stagingBuffer, nullptr);
    m_devFuncs->vkFreeMemory(m_dev, stagingBufferMemory, nullptr);
    

    uint currentFrameSlot = m_window->graphicsStateInfo().currentFrameSlot;

    VkClearValue clearColor = { { {0, 0, 0, 1} } };

    VkRenderPassBeginInfo rpBeginInfo = {};
    rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBeginInfo.renderPass = m_renderPass;
    rpBeginInfo.framebuffer = m_textureFramebuffer;
    rpBeginInfo.renderArea.extent.width = m_size.width();
    rpBeginInfo.renderArea.extent.height = m_size.height();
    rpBeginInfo.clearValueCount = 1;
    rpBeginInfo.pClearValues = &clearColor;

    QSGRendererInterface* rif = m_window->rendererInterface();
    VkCommandBuffer cmdBuf = *reinterpret_cast<VkCommandBuffer*>(
        rif->getResource(m_window, QSGRendererInterface::CommandListResource));

    m_devFuncs->vkCmdBeginRenderPass(cmdBuf, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    m_devFuncs->vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

    VkDeviceSize vbufOffset = 0;
    m_devFuncs->vkCmdBindVertexBuffers(cmdBuf, 0, 1, &m_vbuf, &vbufOffset);

    m_devFuncs->vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1,
        &m_texDescriptor, 0, nullptr);

    VkViewport vp = { 0, 0, float(m_size.width()), float(m_size.height()), 0.0f, 1.0f };
    m_devFuncs->vkCmdSetViewport(cmdBuf, 0, 1, &vp);
    VkRect2D scissor = { { 0, 0 }, { uint32_t(m_size.width()), uint32_t(m_size.height()) } };
    m_devFuncs->vkCmdSetScissor(cmdBuf, 0, 1, &scissor);

    m_devFuncs->vkCmdDraw(cmdBuf, 4, 1, 0, 0);
    m_devFuncs->vkCmdEndRenderPass(cmdBuf);

    // Memory barrier before the texture can be used as a source.
    // Since we are not using a sub-pass, we have to do this explicitly.

    VkImageMemoryBarrier imageTransitionBarrier = {};
    imageTransitionBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageTransitionBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    imageTransitionBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    imageTransitionBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    imageTransitionBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageTransitionBarrier.image = m_texture;
    imageTransitionBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageTransitionBarrier.subresourceRange.levelCount = imageTransitionBarrier.subresourceRange.layerCount = 1;

    m_devFuncs->vkCmdPipelineBarrier(cmdBuf,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0, 0, nullptr, 0, nullptr,
        1, &imageTransitionBarrier);
}

void CustomTextureNodePrivate::CreateTextureImage() {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = static_cast<uint32_t>(m_itemSizeX);
    imageInfo.extent.height = static_cast<uint32_t>(m_itemSizeY);
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
    if (m_devFuncs->vkCreateImage(m_dev, &imageInfo, nullptr, &m_textureImage) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    m_devFuncs->vkGetImageMemoryRequirements(m_dev, m_textureImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (m_devFuncs->vkAllocateMemory(m_dev, &allocInfo, nullptr, &m_textureImageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    m_devFuncs->vkBindImageMemory(m_dev, m_textureImage, m_textureImageMemory, 0);


    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_textureImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (m_devFuncs->vkCreateImageView(m_dev, &viewInfo, nullptr, &m_textureImageView) != VK_SUCCESS)
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

    if (m_devFuncs->vkCreateSampler(m_dev, &samplerInfo, nullptr, &m_textureSampler) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture sampler!");
    }
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

void CustomTextureNodePrivate::prepareShader()
{
    QString filename = QLatin1String(":/qt/qml/com/Application/Shaders/DrawEditorLayout.spv");

    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly))
        qFatal("Failed to read shader %s", qPrintable(filename));

    const QByteArray contents = f.readAll();

    m_shader = contents;
    Q_ASSERT(!m_shader.isEmpty());
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

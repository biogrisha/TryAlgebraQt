#include "VulkanHelpers.h"
#include "VulkanContext.h"
#include <fstream>
vk::raii::ShaderModule VkHelpers::createShaderModule(const std::vector<char>& code, const vk::raii::Device& device)
{
	vk::ShaderModuleCreateInfo createInfo;
	createInfo.codeSize = code.size() * sizeof(char), createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	vk::raii::ShaderModule shaderModule{ device, createInfo };

	return shaderModule;
}

std::vector<char> VkHelpers::readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}
	std::vector<char> buffer(file.tellg());
	file.seekg(0, std::ios::beg);
	file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
	file.close();
	return buffer;
}

vk::raii::CommandBuffer VkHelpers::BeginSingleTimeCommands()
{
	vk::CommandBufferAllocateInfo allocInfo(FVulkanStatic::Context->CommandPool, vk::CommandBufferLevel::ePrimary, 1);
	vk::raii::CommandBuffer commandBuffer = std::move(FVulkanStatic::Context->Device.allocateCommandBuffers(allocInfo).front());

	vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	commandBuffer.begin(beginInfo);

	return commandBuffer;
}

void VkHelpers::EndSingleTimeCommands(vk::CommandBuffer commandBuffer)
{
	commandBuffer.end();

	vk::SubmitInfo submitInfo({}, {}, { commandBuffer });
	FVulkanStatic::Context->GraphicsQueue.submit(submitInfo, nullptr);
	FVulkanStatic::Context->GraphicsQueue.waitIdle();
}

void VkHelpers::CopyBufferRaw(VkBuffer* srcBuffer, VkBuffer* dstBuffer, vk::DeviceSize size)
{
	vk::raii::CommandBuffer commandCopyBuffer = BeginSingleTimeCommands();
	commandCopyBuffer.copyBuffer(*srcBuffer, *dstBuffer, vk::BufferCopy(0, 0, size));
	EndSingleTimeCommands(commandCopyBuffer);
}

void VkHelpers::TransitionImageLayout(const vk::raii::Image& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
	auto commandBuffer = BeginSingleTimeCommands();
	vk::ImageMemoryBarrier barrier({}, {}, oldLayout, newLayout, {}, {}, image, { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });

	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;

	if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
		barrier.srcAccessMask = {};
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}
	commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, {}, barrier);
	EndSingleTimeCommands(commandBuffer);
}

vk::DescriptorType VkHelpers::ConvertBufferToDescriptor(VkBufferUsageFlagBits BufferUsage)
{
	if (BufferUsage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
		return vk::DescriptorType::eUniformBuffer;

	if (BufferUsage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
		return vk::DescriptorType::eStorageBuffer;

	// Optional: handle unsupported usage
	throw std::runtime_error("Unsupported buffer usage for descriptor type.");
}

std::unique_ptr<FBuffer> VkHelpers::ConvertImageToBuffer(FImageBuffer* ImageBuffer)
{
	std::unique_ptr<FBuffer> Buffer = MyRTTI::MakeTypedUnique<FBuffer>();
	FBufferInfo BufferInfo;
	BufferInfo.bDeviceLocal = false;
	BufferInfo.Usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	Buffer->SetProperties(BufferInfo);
	auto ImageExtent = ImageBuffer->GetExtent();
	Buffer->Init(ImageExtent.height * ImageExtent.width * 4);

	ImageTransition_ShaderReadToTransferSrc(ImageBuffer);

	auto CommandBuffer2 = BeginSingleTimeCommands();
	vk::BufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = vk::Offset3D{ 0, 0, 0 };
	region.imageExtent = vk::Extent3D{ ImageExtent.width, ImageExtent.height, 1 };

	CommandBuffer2.copyImageToBuffer(ImageBuffer->GetImage(), vk::ImageLayout::eTransferSrcOptimal, *Buffer->GetBuffer(), region);
	
	EndSingleTimeCommands(CommandBuffer2);
	ImageTransition_TransferSrcToShaderRead(ImageBuffer);
	return Buffer;
}

void VkHelpers::ImageTransition_ShaderReadToTransferSrc(FImageBuffer* Image)
{
	auto CommandBuffer = VkHelpers::BeginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = Image->GetImage();
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	// Access masks depend on old usage:
	barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

	vkCmdPipelineBarrier(
		*CommandBuffer,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);
	VkHelpers::EndSingleTimeCommands(CommandBuffer);
}

void VkHelpers::ImageTransition_TransferSrcToShaderRead(FImageBuffer* Image)
{
	auto CommandBuffer = VkHelpers::BeginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = Image->GetImage();
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	// Access masks depend on old usage:
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(
		*CommandBuffer,
		VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);
	VkHelpers::EndSingleTimeCommands(CommandBuffer);
}

VkShaderModule NVkHelpers::createShaderModule(const uint32_t* code, uint32_t size, VkDevice device)
{
	VkShaderModuleCreateInfo createInfo;
	memset(&createInfo, 0, sizeof(createInfo));
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.flags = 0;
	createInfo.codeSize = size;
	createInfo.pCode = code;
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}
	return shaderModule;
}
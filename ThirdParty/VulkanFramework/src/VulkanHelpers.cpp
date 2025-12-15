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
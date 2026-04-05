#include "SpriteRendering.h"
#include "ImageBuffer.h"
#include "VulkanHelpers.h"
#include "FileSystemUtilities.h"


namespace {
	uint16_t S_1;
	uint16_t P_1;
	uint16_t PLine;
}

void FSpriteRendering::Init(FRendering* InRendering)
{
	Rendering = InRendering;
	//Create resources
	{
		VertexBuffer = MyRTTI::MakeTypedUnique<FBuffer>();
		FBufferInfo Info;
		Info.bDeviceLocal = true;
		Info.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		VertexBuffer->SetProperties(Info);
		VertexBuffer->SetData(RectVertices);
	}
	{
		IndexBuffer = MyRTTI::MakeTypedUnique<FBuffer>();
		FBufferInfo Info;
		Info.bDeviceLocal = true;
		Info.Usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		IndexBuffer->SetProperties(Info);
		IndexBuffer->SetData(RectIndices);
	}
	{
		auto AssetsPath = FSUtils::getAssetsPath();
		Atlas.SetPath(AssetsPath + "/Atlases/MathEditorAtlas");
		Atlas.Init();
	}
	{
		InstanceBuffer = MyRTTI::MakeTypedUnique<FBuffer>();
		FBufferInfo Info;
		Info.bDeviceLocal = true;
		Info.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		InstanceBuffer->SetProperties(Info);
	}
	{
		UniformBuffer = MyRTTI::MakeTypedUnique<FBuffer>();
		UniformBuffer->SetProperties({ VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT , false });
		UniformBuffer->SetData(sizeof(Extent), &Extent);
	}
	{
		FImageBufferInfo image_info;
		image_info.Extent = Extent;
		image_info.UsageFlags |= vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst;
		Output = MyRTTI::MakeTypedUnique<FImageBuffer>(image_info);
		Output->Init();
	}

	S_1 = Rendering->GetDescriptorManager().MakeDescriptorSet({
				{Atlas.GetImage(), vk::ShaderStageFlagBits::eFragment},
				{UniformBuffer.get(), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
		});

	P_1 = Rendering->GetDescriptorManager().MakePipelineLayout({ S_1 });
}

void FSpriteRendering::InitPLine()
{
	auto AssetsPath = FSUtils::getAssetsPath();
	PLine = Rendering->AddPipeline(P_1, &SpriteLayout, AssetsPath + "/Shader/DrawSprites.spv");
}

void FSpriteRendering::SetExtent(const VkExtent3D& InExtent)
{
	Extent = InExtent;
	if (UniformBuffer)
	{
		UniformBuffer->SetData(sizeof(Extent), &Extent);
		Output->SetExtent(InExtent);
	}
}

void FSpriteRendering::SetInput(FImageBuffer* InInput)
{
	Input = InInput;
}

void FSpriteRendering::Render()
{
	auto CommandBuffer = VkHelpers::BeginSingleTimeCommands();
	VkHelpers::ImageTransition_ToTransferSrc(Input, CommandBuffer, vk::PipelineStageFlagBits::eFragmentShader);
	VkHelpers::ImageTransition_ToTransferDst(Output.get(), CommandBuffer, vk::PipelineStageFlagBits::eFragmentShader);
	VkHelpers::CopyImageToImage(Input, Output.get(), CommandBuffer);
	VkHelpers::ImageTransition_ToShaderRead(Input, CommandBuffer, vk::PipelineStageFlagBits::eTransfer);
	VkHelpers::ImageTransition_ToShaderRead(Output.get(), CommandBuffer, vk::PipelineStageFlagBits::eTransfer);
	VkHelpers::EndSingleTimeCommands(CommandBuffer);
	FRunPipelineInfo Run;
	Run.PipelineId = PLine;
	Run.OutputExtent = Extent;
	Run.VertexBuffers = { VertexBuffer.get(), InstanceBuffer.get() };
	Run.IndexBuffer = IndexBuffer.get();
	Run.DescriptorSets = { S_1 };
	Run.ColorAttachment = Output.get();
	Run.IndicesCount = RectIndices.size();
	Run.InstancesCount = InstancesCount;
	Run.bClearAttachment = false;
	Rendering->AddRunPipelineInfo(Run);
	Rendering->Render();
}

void FSpriteRendering::SetInstances(const std::vector<FSpriteInstByName>& Sprites)
{
	std::vector<FSpriteInst> Instances;
	for (auto& Sprite : Sprites)
	{
		auto SpriteInfo = Atlas.GetInfo(Sprite.SpriteName);
		auto& Inst = Instances.emplace_back();
		Inst.Pos = Sprite.Pos;
		Inst.Size = Sprite.Size;
		Inst.TexPos = SpriteInfo.TexPos;
		Inst.TexSize = SpriteInfo.TexSize;
		Inst.Alpha = Sprite.Alpha;
	}
	InstanceBuffer->SetData(Instances);
	InstancesCount = Sprites.size();
}

FImageBuffer* FSpriteRendering::GetResult()
{
	return Output.get();
}

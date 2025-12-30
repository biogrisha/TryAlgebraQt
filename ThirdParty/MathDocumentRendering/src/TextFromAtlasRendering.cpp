#include "TextFromAtlasRendering.h"
#include <VulkanHelpers.h>

namespace {
	uint16_t S_1;
	uint16_t P_1;
	uint16_t PLine;
}

void FTextFromAtlasRendering::Init(FRendering* InRendering)
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
		InstanceBuffer = MyRTTI::MakeTypedUnique<FBuffer>();
		FBufferInfo Info;
		Info.bDeviceLocal = true;
		Info.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		InstanceBuffer->SetProperties(Info);
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
		Result = MyRTTI::MakeTypedUnique<FImageBuffer>();
		Result->SetExtent(Extent);
		Result->AddUsageFlags(VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
		Result->Init();
	}
	{
		UniformBuffer = MyRTTI::MakeTypedUnique<FBuffer>();
		UniformBuffer->SetProperties({ VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT , false });
		UniformBuffer->SetData(sizeof(Extent), &Extent);
	}

	S_1 = Rendering->GetDescriptorManager().MakeDescriptorSet({
				{AtlasBuffer, vk::ShaderStageFlagBits::eFragment},
				{UniformBuffer.get(), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
			});

	P_1 = Rendering->GetDescriptorManager().MakePipelineLayout({ S_1 });
}

void FTextFromAtlasRendering::InitPLine()
{
	PLine = Rendering->AddPipeline(P_1, &TextFromAtlasLayout, "D:/Projects/TryAlgebraQt/TryAlgebraQt/ThirdParty/Shader/TextFromAtlas.spv");
}

void FTextFromAtlasRendering::SetExtent(const VkExtent2D& InExtent)
{
	if (Result)
	{
		Result->SetExtent(InExtent);
		UniformBuffer->SetData(sizeof(InExtent), &InExtent);
	}
	Extent = InExtent;
	
}
void FTextFromAtlasRendering::SetAtlas(FImageBuffer* InAtlasBuffer)
{
	AtlasBuffer = InAtlasBuffer;
}

void FTextFromAtlasRendering::SetInstances(const std::vector<FSpriteInstance>& InInstances)
{
	InstanceBuffer->SetData(InInstances);
	InstancesCount = InInstances.size();
}

FImageBuffer* FTextFromAtlasRendering::Render()
{
	FRunPipelineInfo Run;

	Run.PipelineId = PLine;
	Run.OutputExtent = Extent;
	Run.VertexBuffers = { VertexBuffer.get(), InstanceBuffer.get()};
	Run.IndexBuffer = IndexBuffer.get();
	Run.DescriptorSets = { S_1 };
	Run.ColorAttachment = Result.get();
	Run.IndicesCount = RectIndices.size();
	Run.InstancesCount = InstancesCount;

	Rendering->AddRunPipelineInfo(Run);
	Rendering->Render();
	return Result.get();
}

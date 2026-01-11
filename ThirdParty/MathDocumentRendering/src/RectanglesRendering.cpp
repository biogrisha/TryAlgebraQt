#include "RectanglesRendering.h"
#include "ImageBuffer.h"
#include "VulkanHelpers.h"
#include "FileSystemUtilities.h"

namespace {
	uint16_t S_1;
	uint16_t P_1;
	uint16_t PLine;
}

void FRectRendering::Init(FRendering* InRendering)
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
		Output = MyRTTI::MakeTypedUnique<FImageBuffer>();
		Output->SetExtent(Extent);
		Output->AddUsageFlags(VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
		Output->Init();
	}

	S_1 = Rendering->GetDescriptorManager().MakeDescriptorSet({
				{UniformBuffer.get(), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
		});

	P_1 = Rendering->GetDescriptorManager().MakePipelineLayout({ S_1 });
}

void FRectRendering::InitPLine()
{
	auto AssetsPath = FSUtils::getAssetsPath();
	PLine = Rendering->AddPipeline(P_1, &RectLayout, AssetsPath + "/Shader/DrawRectangles.spv");
}

void FRectRendering::SetExtent(const VkExtent2D& InExtent)
{
	if (Output)
	{
		Output->SetExtent(InExtent);
		UniformBuffer->SetData(sizeof(InExtent), &InExtent);
	}
	Extent = InExtent;
}

void FRectRendering::Render()
{
	if (InstancesCount == 0)
	{
		return;
	}
	FRunPipelineInfo Run;
	Run.PipelineId = PLine;
	Run.OutputExtent = Extent;
	Run.VertexBuffers = { VertexBuffer.get(), InstanceBuffer.get() };
	Run.IndexBuffer = IndexBuffer.get();
	Run.DescriptorSets = { S_1 };
	Run.ColorAttachment = Output.get();
	Run.IndicesCount = RectIndices.size();
	Run.InstancesCount = InstancesCount;
	Rendering->AddRunPipelineInfo(Run);
	Rendering->Render();
}

void FRectRendering::SetInstances(const std::vector<FRectInst>& Rects)
{
	InstancesCount = Rects.size();
	if (Rects.empty())
	{
		return;
	}
	InstanceBuffer->SetData(Rects);
}

FImageBuffer* FRectRendering::GetResult()
{
	return Output.get();
}

bool FRectRendering::HasInstances()
{
	return InstancesCount > 0;
}


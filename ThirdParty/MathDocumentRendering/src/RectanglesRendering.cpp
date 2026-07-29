#include "RectanglesRendering.h"
#include "ImageBuffer.h"
#include "VulkanHelpers.h"
#include "FileSystemUtilities.h"

void FRectRendering::Init(FRendering* InRendering, FImageBuffer* output)
{
	Rendering = InRendering;
	m_output = output;
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

void FRectRendering::setExtent(const VkExtent3D& InExtent)
{
	if (UniformBuffer)
	{
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
	Run.ColorAttachment = m_output;
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
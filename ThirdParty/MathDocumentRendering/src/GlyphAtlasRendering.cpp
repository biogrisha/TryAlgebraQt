#include "GlyphAtlasRendering.h"
#include <VulkanHelpers.h>
#include <VulkanContext.h>
#include "FileSystemUtilities.h"
namespace 
{
	uint16_t S_1,P_1,PLine;
}

void FGlyphAtlasRendering::Init(FRendering* InRendering)
{
	Rendering = InRendering;
	//Creating resources
	VertexBuffer = MyRTTI::MakeTypedUnique<FBuffer>();
	VertexBuffer->SetProperties(FBufferInfo{ VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, true });
	VertexBuffer->SetData(RectVertices);

	IndexBuffer = MyRTTI::MakeTypedUnique<FBuffer>();
	IndexBuffer->SetProperties(FBufferInfo{ VK_BUFFER_USAGE_INDEX_BUFFER_BIT , true });
	IndexBuffer->SetData(RectIndices);

	InstanceBuffer = MyRTTI::MakeTypedUnique<FBuffer>();
	InstanceBuffer->SetProperties(FBufferInfo{ VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, true });

	OutlineBuffer = MyRTTI::MakeTypedUnique<FBuffer>();
	OutlineBuffer->SetProperties(FBufferInfo(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, true));

	UniformBuffer = MyRTTI::MakeTypedUnique<FBuffer>();
	UniformBuffer->SetProperties({ VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT , false });
	UniformBuffer->SetData(sizeof(Extent), &Extent);

	Atlas = MyRTTI::MakeTypedUnique<FImageBuffer>();
	Atlas->SetExtent(Extent);
	Atlas->AddUsageFlags(VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
	Atlas->Init();

	S_1 = Rendering->GetDescriptorManager().MakeDescriptorSet(
		{
			{OutlineBuffer.get(),vk::ShaderStageFlagBits::eFragment},
			{UniformBuffer.get(),vk::ShaderStageFlagBits::eVertex},
		});

	P_1 = Rendering->GetDescriptorManager().MakePipelineLayout({ S_1 });
}

void FGlyphAtlasRendering::InitPLine()
{
	auto AssetsPath = FSUtils::getAssetsPath();
	PLine = Rendering->AddPipeline(P_1, &GlyphAtlasInputLayout, AssetsPath + "/Shader/DrawAtlas.spv");
}

void FGlyphAtlasRendering::SetExtent(const VkExtent2D& InExtent)
{
	if (Atlas)
	{
		Atlas->SetExtent(InExtent);
		UniformBuffer->SetData(sizeof(InExtent), &InExtent);
	}
	Extent = InExtent;
}
void FGlyphAtlasRendering::SetInstances(const std::vector<FGlyphInstance>& InInstances)
{
	Instances = InInstances;
	InstanceBuffer->SetData(Instances);
}

void FGlyphAtlasRendering::SetOutlineCurves(const std::vector<FOutlineCurvePoints>& InOutlineCurves)
{
	OutlineCurves = InOutlineCurves;
	OutlineBuffer->SetData(OutlineCurves);
}

void FGlyphAtlasRendering::Render()
{
	FRunPipelineInfo Run;

	Run.PipelineId = PLine;
	Run.OutputExtent = Extent;
	Run.VertexBuffers = { VertexBuffer.get(), InstanceBuffer.get() };
	Run.IndexBuffer = IndexBuffer.get();
	Run.DescriptorSets = { S_1 };
	Run.ColorAttachment = Atlas.get();
	Run.IndicesCount = RectIndices.size();
	Run.InstancesCount = Instances.size();

	Rendering->AddRunPipelineInfo(Run);
	Rendering->Render();
}

FImageBuffer* FGlyphAtlasRendering::GetAtlas()
{
	return Atlas.get();
}

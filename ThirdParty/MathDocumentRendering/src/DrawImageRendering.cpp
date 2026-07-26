#include "DrawImageRendering.h"
#include <FileSystemUtilities.h>
#include <VulkanHelpers.h>

void DrawImageRendering::Init(FRendering* rendering, FImageBuffer* input, FImageBuffer* output)
{
	m_rendering = rendering;
	m_output = output;
	//Create resources
	{
		m_vertexBuffer = MyRTTI::MakeTypedUnique<FBuffer>();
		FBufferInfo Info;
		Info.bDeviceLocal = true;
		Info.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		m_vertexBuffer->SetProperties(Info);
		m_vertexBuffer->SetData(m_rectVertices);
	}
	{
		m_indexBuffer = MyRTTI::MakeTypedUnique<FBuffer>();
		FBufferInfo Info;
		Info.bDeviceLocal = true;
		Info.Usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		m_indexBuffer->SetProperties(Info);
		m_indexBuffer->SetData(m_rectIndices);
	}

	SetHndl = m_rendering->GetDescriptorManager().MakeDescriptorSet({
				{input, vk::ShaderStageFlagBits::eFragment},
		});

	PLineLayoutHndl = m_rendering->GetDescriptorManager().MakePipelineLayout({ SetHndl });

}

void DrawImageRendering::InitPLine()
{
	auto assetsPath = FSUtils::getAssetsPath();
	PLineHndl = m_rendering->AddPipeline(PLineLayoutHndl, &m_vertexLayout, assetsPath + "/Shader/DrawImage.spv");
}

void DrawImageRendering::setExtent(const vk::Extent3D& extent)
{
	m_extent = extent;
}

void DrawImageRendering::Render(bool clearAttachment)
{
	FRunPipelineInfo run;
	run.PipelineId = PLineHndl;
	run.OutputExtent = m_extent;
	run.VertexBuffers = { m_vertexBuffer.get() };
	run.IndexBuffer = m_indexBuffer.get();
	run.DescriptorSets = { SetHndl };
	run.ColorAttachment = m_output;
	run.IndicesCount = m_rectIndices.size();
	run.InstancesCount = 1;
	run.bClearAttachment = clearAttachment;
	m_rendering->AddRunPipelineInfo(run);
	m_rendering->Render();
}

#include "LinesRendering.h"
#include <FileSystemUtilities.h>

void LinesRendering::Init(FRendering* InRendering, FImageBuffer* InOutput)
{
	m_initialized = true;
	m_rendering = InRendering;
	//Create resources
	{
		m_vertexBuffer = MyRTTI::MakeTypedUnique<FBuffer>();
		FBufferInfo Info;
		Info.bDeviceLocal = true;
		Info.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		m_vertexBuffer->SetProperties(Info);
	}
	{
		m_indexBuffer = MyRTTI::MakeTypedUnique<FBuffer>();
		FBufferInfo Info;
		Info.bDeviceLocal = true;
		Info.Usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		m_indexBuffer->SetProperties(Info);
	}
	{
		m_uniformBuffer = MyRTTI::MakeTypedUnique<FBuffer>();
		m_uniformBuffer->SetProperties({ VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT , false });
		m_uniformBuffer->SetData(sizeof(m_extent), &m_extent);
	}
	m_output = InOutput;

	DSetHndl = m_rendering->GetDescriptorManager().MakeDescriptorSet({
			{m_uniformBuffer.get(), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
		});
	PLineLayoutHndl = m_rendering->GetDescriptorManager().MakePipelineLayout({ DSetHndl });
}

void LinesRendering::InitPLine()
{
	auto assetsPath = FSUtils::getAssetsPath();
	PLineHndl = m_rendering->AddPipeline(PLineLayoutHndl, &m_lineLayout, assetsPath + "/Shader/DrawPolygons.spv");
}

void LinesRendering::setExtent(const VkExtent3D& InExtent)
{
	if (m_initialized)
	{
		m_uniformBuffer->SetData(sizeof(InExtent), &InExtent);
	}
	m_extent = InExtent;
}

void LinesRendering::setInstances(std::vector<LineChain> instances)
{
	m_chains = std::move(instances);
}

void LinesRendering::Render()
{
	if (m_chains.empty())
	{
		return;
	}
	flushIntoBuffer();
	FRunPipelineInfo run;
	run.PipelineId = PLineHndl;
	run.OutputExtent = m_extent;
	run.VertexBuffers = { m_vertexBuffer.get() };
	run.IndexBuffer = m_indexBuffer.get();
	run.DescriptorSets = { DSetHndl };
	run.ColorAttachment = m_output;
	run.IndicesCount = m_indCount;
	run.InstancesCount = 1;
	run.bClearAttachment = false;
	m_rendering->AddRunPipelineInfo(run);
	m_rendering->Render();
}

void LinesRendering::flushIntoBuffer()
{
	std::vector<FVertColored> vertices;
	std::vector<uint16_t> indices;
	for (auto& chain : m_chains)
	{
		float halfWidth = chain.width / 2;
		auto& points = chain.points;
		for (int i = 0; i < points.size() - 1; ++i)
		{
			auto& from = points[i];
			auto& to = points[i + 1];
			auto dir = to - from;
			auto perp = glm::normalize(glm::vec2(dir.y, -dir.x)) * halfWidth;

			int startIndex = vertices.size();
			vertices.emplace_back(from + perp, chain.color);
			vertices.emplace_back(from - perp, chain.color);
			vertices.emplace_back(to + perp, chain.color);
			vertices.emplace_back(to - perp, chain.color);

			indices.push_back(startIndex);
			indices.push_back(startIndex + 1);
			indices.push_back(startIndex + 3);
			indices.push_back(startIndex);
			indices.push_back(startIndex + 3);
			indices.push_back(startIndex + 2);
		}
	}
	m_indCount = indices.size();
	m_chains.clear();
	m_vertexBuffer->SetData(vertices);
	m_indexBuffer->SetData(indices);
}

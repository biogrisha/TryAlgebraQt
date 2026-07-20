#include "LinesRendering.h"
#include <FileSystemUtilities.h>
namespace {
	int PLineLayoutHndl;
	int PLineHndl;
}
void LinesRendering::Init(FRendering* InRendering, FImageBuffer* InOutput)
{
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
	PLineLayoutHndl = m_rendering->GetDescriptorManager().MakePipelineLayout({ });
}

void LinesRendering::InitPLine()
{
	auto assetsPath = FSUtils::getAssetsPath();
	PLineHndl = m_rendering->AddPipeline(PLineLayoutHndl, &m_lineLayout, assetsPath + "/Shader/DrawRectangles.spv");
}

void LinesRendering::SetExtent(const VkExtent3D& InExtent)
{
	m_uniformBuffer->SetData(sizeof(InExtent), &InExtent);
	m_extent = InExtent;
}

void LinesRendering::Render()
{
	FRunPipelineInfo Run;
	Run.PipelineId = PLineHndl;
	Run.OutputExtent = m_extent;
	Run.VertexBuffers = { VertexBuffer.get(), InstanceBuffer.get() };
	Run.IndexBuffer = IndexBuffer.get();
	Run.DescriptorSets = { S_1 };
	Run.ColorAttachment = Output.get();
	Run.IndicesCount = RectIndices.size();
	Run.InstancesCount = InstancesCount;
	Rendering->AddRunPipelineInfo(Run);
	Rendering->Render();
}

void LinesRendering::addChain(LinesChain chain)
{
	m_chains.push_back(std::move(chain));
}

void LinesRendering::flushIntoBuffer()
{
	std::vector<FVertColored> vertices;
	std::vector<int> indices;
	for (auto& chain : m_chains)
	{
		float halfWidth = chain.width / 2;
		auto& points = chain.points;
		//initialize first line segment
		{
			int startIndex = 0;
			auto& from = points[0];
			auto& to = points[1];
			auto dir = to - from;
			auto perp = glm::normalize(glm::vec2(dir.y, -dir.x)) * halfWidth;

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
		for (int i = 1; i < points.size() - 1; ++i)
		{
			int startIndex = indices.size();
			auto& from = points[0];
			auto& to = points[1];
			auto dir = to - from;
			auto perp = glm::normalize(glm::vec2(dir.y, -dir.x)) * halfWidth;

			vertices.emplace_back(from + perp, chain.color);
			vertices.emplace_back(from - perp, chain.color);
			vertices.emplace_back(to + perp, chain.color);
			vertices.emplace_back(to - perp, chain.color);

			//wedge on the bend
			indices.push_back(startIndex);
			indices.push_back(startIndex - 1);
			indices.push_back(startIndex + 1);
			//line segment
			indices.push_back(startIndex);
			indices.push_back(startIndex + 1);
			indices.push_back(startIndex + 3);
			indices.push_back(startIndex);
			indices.push_back(startIndex + 3);
			indices.push_back(startIndex + 2);
		}
	}
	m_chains.clear();
	m_vertexBuffer->SetData(vertices);
	m_indexBuffer->SetData(indices);
}

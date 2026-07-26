#include "SpriteRendering.h"
#include "ImageBuffer.h"
#include "VulkanHelpers.h"
#include "FileSystemUtilities.h"


void FSpriteRendering::Init(FRendering* InRendering, FImageBuffer* output)
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

void FSpriteRendering::setExtent(const VkExtent3D& InExtent)
{
	Extent = InExtent;
	if (UniformBuffer)
	{
		UniformBuffer->SetData(sizeof(Extent), &Extent);
	}
}

void FSpriteRendering::Render()
{
	if (!m_hasContent)
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
	Run.bClearAttachment = false;
	Rendering->AddRunPipelineInfo(Run);
	Rendering->Render();
}

void FSpriteRendering::SetInstances(const std::vector<FSpriteInstByName>& Sprites)
{
	m_hasContent = !Sprites.empty();
	if (!m_hasContent)
	{
		return;
	}

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


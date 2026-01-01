#include "SpriteRendering.h"
#include "ImageBuffer.h"

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
		Atlas.SetPath("D:/Projects/TryAlgebraQt/TryAlgebraQt/RawAssets/Atlases/MathEditorAtlas");
		Atlas.Init();
	}
	{
		InstanceBuffer = MyRTTI::MakeTypedUnique<FBuffer>();
		FBufferInfo Info;
		Info.bDeviceLocal = true;
		Info.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		InstanceBuffer->SetProperties(Info);
		auto SpriteInfo = Atlas.GetInfo("TextCaret.png");
		FSpriteInst Inst;
		Inst.Pos = { 10,10 };
		Inst.Size = { 100, 200 };
		Inst.TexPos = SpriteInfo.TexPos;
		Inst.TexSize = SpriteInfo.TexSize;
		InstanceBuffer->SetData(std::vector{ Inst });
		InstancesCount = 1;
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
	PLine = Rendering->AddPipeline(P_1, &SpriteLayout, "D:/Projects/TryAlgebraQt/TryAlgebraQt/ThirdParty/Shader/DrawSprites.spv");
}

void FSpriteRendering::SetExtent(const VkExtent2D& InExtent)
{
	Extent = InExtent;
}

void FSpriteRendering::SetInput(FImageBuffer* InInputTexture)
{
	InputTextrure = InInputTexture;
}

void FSpriteRendering::Render()
{
	FRunPipelineInfo Run;

	Run.PipelineId = PLine;
	Run.OutputExtent = Extent;
	Run.VertexBuffers = { VertexBuffer.get(), InstanceBuffer.get() };
	Run.IndexBuffer = IndexBuffer.get();
	Run.DescriptorSets = { S_1 };
	Run.ColorAttachment = InputTextrure;
	Run.IndicesCount = RectIndices.size();
	Run.InstancesCount = InstancesCount;
	Run.bClearAttachment = false;
	Rendering->AddRunPipelineInfo(Run);
	Rendering->Render();
}

FImageBuffer* FSpriteRendering::GetResult()
{
	return InputTextrure;
}

#include "MathDocumentRendering.h"
#include <stdexcept>
#include <iostream>
#include <MathLibrary.h>
#include <VulkanContext.h>
#include <chrono>
#include <algorithm>
#include "VulkanHelpers.h"

FMathDocumentRendering::FMathDocumentRendering()
	:m_state(FMathDocumentState(2))
{
}

void FMathDocumentRendering::Init(FFreeTypeWrap* ft)
{
	m_rendering = std::make_unique<FRendering>();

	{
		FImageBufferInfo layerInfo;
		layerInfo.Extent = m_extent;
		layerInfo.UsageFlags = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc;
		m_output = MyRTTI::MakeTypedUnique<FImageBuffer>(layerInfo);
	}

	{
		FImageBufferInfo layerInfo;
		layerInfo.Extent = m_extent;
		layerInfo.UsageFlags = vk::ImageUsageFlagBits::eColorAttachment
			| vk::ImageUsageFlagBits::eSampled
			| vk::ImageUsageFlagBits::eTransferDst
			| vk::ImageUsageFlagBits::eTransferSrc;
		m_layer1 = MyRTTI::MakeTypedUnique<FImageBuffer>(layerInfo);
		m_layer2 = MyRTTI::MakeTypedUnique<FImageBuffer>(layerInfo);
	}

	//layer 1
	m_rectRendering1.Init(m_rendering.get(), m_layer1.get());

	//layer 2 
	m_spriteRendering2.Init(m_rendering.get(), m_layer2.get());
	m_rectRendering2.Init(m_rendering.get(), m_layer2.get());
	m_linesRendering2.Init(m_rendering.get(), m_layer2.get());
	m_textRendering2.init(m_rendering.get(), m_layer2.get(), ft);

	m_layer1ToOutput.Init(m_rendering.get(), m_layer1.get(), m_output.get());
	m_layer2ToOutput.Init(m_rendering.get(), m_layer2.get(), m_output.get());

	m_rendering->GetDescriptorManager().Init();

	//layer 1
	m_rectRendering1.InitPLine();

	//layer 2 
	m_spriteRendering2.InitPLine();
	m_rectRendering2.InitPLine();
	m_linesRendering2.InitPLine();
	m_textRendering2.initPLine();

	m_layer1ToOutput.InitPLine();
	m_layer2ToOutput.InitPLine();
}

void FMathDocumentRendering::SetDocumentExtent(const VkExtent3D& extent)
{
	m_extent = extent;
	if (m_layer1)
	{
		m_layer1->SetExtent(extent);
	}
	if (m_layer2)
	{
		m_layer2->SetExtent(extent);
	}
	m_rectRendering1.setExtent(extent);
	m_spriteRendering2.setExtent(extent);
	m_rectRendering2.setExtent(extent);
	m_linesRendering2.setExtent(extent);
	m_textRendering2.setExtent(extent);
	m_layer1ToOutput.setExtent(extent);
	m_layer2ToOutput.setExtent(extent);
}


FImageBuffer* FMathDocumentRendering::Render()
{
	if (m_state.at(0).dirty())
	{
		auto cmdBuffer = VkHelpers::BeginSingleTimeCommands();
		VkHelpers::ImageTransition_ToTransferDst(m_layer1.get(), cmdBuffer);
		VkHelpers::ClearImage(m_layer1.get(), cmdBuffer);
		VkHelpers::EndSingleTimeCommands(cmdBuffer);

		m_rectRendering1.SetInstances(m_state.at(0).rectangles());

		m_rectRendering1.Render();
		m_layer1ToOutput.Render(true);
	}
	if (m_state.at(1).dirty())
	{
		m_spriteRendering2.SetInstances(m_state.at(1).sprites());
		m_rectRendering2.SetInstances(m_state.at(1).rectangles());
		m_linesRendering2.setInstances(m_state.at(1).lines());
		m_textRendering2.updateText(m_state.at(1).text());

		m_spriteRendering2.Render();
		m_rectRendering2.Render();
		m_linesRendering2.Render();
		m_textRendering2.render();
		m_layer2ToOutput.Render(false);
	}
	return m_output.get();
}

FMathDocumentState* FMathDocumentRendering::getState()
{
	return &m_state;
}
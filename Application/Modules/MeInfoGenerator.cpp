#include "MeInfoGenerator.h"
#include <Me/include/MeAtlas.h>
#include <Me/include/MeParser.h>
#include <Me/include/MeGlobals.h>
#include <MathEditor/include/TextBuffer.h>
#include <Application.h>
#include <AppGlobal.h>
#include <MathDocumentState.h>
#include <MathDocumentRendering.h>
#include <VulkanHelpers.h>
#include <QImage>

void MeInfoGenerator::gen(MeListModel* model)
{
	using namespace TryAlgebraCore;
	auto atlas = MyRTTI::MakeTypedUnique<TryAlgebraCore::MeAtlas>(500);
	auto& meTable = TryAlgebraCore::MeNames::getMeTable();

	TextBuffer tb;
	for(auto& def : meTable)
	{
		tb.insert(def.second, tb.getSize());
	}
	MeParser pr(tb, 0);
	while (pr.parseLine(atlas.get()));

	FMathDocumentState me_doc_state;
	VisualToolkit vt;
	vt.ft = AppGlobal::application->getFreeTypeWrap();
	vt.mdocState = &me_doc_state;
	atlas->calculate(&vt);
	atlas->draw(&vt);
	vt.mdocState->SetCaret({ .Pos = { -100, -100},.Size = {0,0} });
	uint32_t width = static_cast<uint32_t>(atlas->getSize().x);
	uint32_t height = static_cast<uint32_t>(atlas->getSize().y);

	FMathDocumentRendering md_rendering;
	md_rendering.SetDocumentExtent({ width, height, 1 });
	md_rendering.Init(vt.ft);
	md_rendering.UpdateState(me_doc_state);
	auto RenderedDocument = md_rendering.Render();
	auto RenderedDocBuffer = VkHelpers::ConvertImageToBuffer(RenderedDocument);

	void* RenderedDocData = RenderedDocBuffer->MapData();

	QImage image(
		static_cast<int>(width),
		static_cast<int>(height),
		QImage::Format_RGBA8888
	);
	const int srcBytesPerLine = static_cast<int>(width * 4);
	const int dstBytesPerLine = image.bytesPerLine();

	auto* src = static_cast<const uchar*>(RenderedDocData);

	for (uint32_t y = 0; y < height; ++y)
	{
		std::memcpy(
			image.scanLine(static_cast<int>(y)),
			src + y * srcBytesPerLine,
			srcBytesPerLine
		);
	}
	RenderedDocBuffer->UnmapData();

	auto& children = atlas->getChildren();
	assert(children.size() == meTable.size());
	for (int i = 0; i < children.size(); ++i)
	{
		QPoint pos{ int(children[i]->getPos().x), int(children[i]->getPos().y) };
		QSize size{ int(children[i]->getSize().x), int(children[i]->getSize().y) };
		model->addMathElementInfo({ QString(meTable[i].first), pos, size });
	}
	model->setImage(std::move(image));
}
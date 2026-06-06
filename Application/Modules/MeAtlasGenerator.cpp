#include "MeAtlasGenerator.h"
#include <Me/include/MeAtlas.h>
#include <Me/include/MeParser.h>
#include <MathEditor/include/TextBuffer.h>
#include <Application.h>
#include <AppGlobal.h>
#include <MathDocumentState.h>
#include <MathDocumentRendering.h>
#include <VulkanHelpers.h>
#include <QImage>
void MeAtlasGenerator::gen()
{
	using namespace TryAlgebraCore;
	auto atlas = MyRTTI::MakeTypedUnique<TryAlgebraCore::MeAtlas>(500);
	std::wstring str = L"\\ft\\∫\\{\\,\\}";
	str += L"\\ft\\∬\\{\\,\\}";
	str += L"\\ft\\∭\\{\\,\\}";
	str += L"∀";
	str += L"∃";

	TextBuffer tb;
	tb.insert(str, 0);
	MeParser pr(tb, 0);
	while (pr.parseLine(atlas.get()));

	FMathDocumentState me_doc_state;
	VisualToolkit vt;
	vt.ft = AppGlobal::application->getFreeTypeWrap();
	vt.mdoc_state = &me_doc_state;
	atlas->calculate(&vt);
	atlas->draw(&vt);
	vt.mdoc_state->SetCaret({ .Pos = { -100, -100},.Size = {0,0} });
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
	image = image.mirrored(false, true);
	bool ok = image.save("C:/Users/biogr/OneDrive/Desktop/test.png");


}
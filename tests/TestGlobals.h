#pragma once
#include <MathEditor/include/MathDocument.h>
#include <FreeTypeWrap.h>
#include <MathDocumentState.h>
namespace TestGlobals
{
	inline uint32_t XDpi = 94;
	inline uint32_t YDpi = 94;

	struct ContCalculated
	{
		ContCalculated(const std::wstring& str)
		{
			ft.Init(XDpi, XDpi);
			vt.ft = &ft;
			vt.mdocState = &md;

			cont = TestHelpers::parse(str);
			cont->calculate(&vt);
			cont->calculatePos();
		}
		FFreeTypeWrap ft;
		FMathDocumentState md;
		VisualToolkit vt;
		std::unique_ptr<MeContainer> cont;
	};
}
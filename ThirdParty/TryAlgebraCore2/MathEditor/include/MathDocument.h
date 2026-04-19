#pragma once
#include <MathEditor/include/TextBuffer.h>
#include <MathDocumentRenderingStructs.h>
#include <FreeTypeWrap.h>

namespace TryAlgebraCore2
{
	class MathDocument
	{
	public:
		MathDocument();
		void type(const std::wstring& str);
		std::vector<FGlyphData> getRenderingData(FFreeTypeWrap* ft);
		TextBuffer text_buffer;
		std::vector<int> caret_path;
	};
}
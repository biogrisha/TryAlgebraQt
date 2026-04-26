#pragma once
#include <MathEditor/include/TextBuffer.h>
#include <MathDocumentState.h>

namespace TryAlgebraCore2
{
	class MathDocument
	{
	public:
		MathDocument();
		void type(const std::wstring& str);
		void delBackward();
		void delForward();
		void stepLeft();
		void stepRight();
		void draw(VisualToolkit* visual_toolkit);
		TextBuffer m_text_buffer;
		int m_current_pos;
	};
}
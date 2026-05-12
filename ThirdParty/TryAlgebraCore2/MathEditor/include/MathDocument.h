#pragma once
#include <MathEditor/include/TextBuffer.h>
#include <Me/include/MeContainer.h>
#include <MathDocumentState.h>

namespace TryAlgebraCore2
{
	class MathDocument
	{
	public:
		MathDocument();
		void setDocHeight(float height) { m_doc_height = height; }
		void setText(const std::wstring& str);
		void type(const std::wstring& str);
		void delBackward();
		void delForward();
		void stepLeft();
		void stepRight();
		void draw(VisualToolkit* visual_toolkit);
		TextBuffer m_text_buffer;
		int m_current_pos;
		std::unique_ptr<MeContainer> m_container;
		float m_doc_height = 0;
	};
}
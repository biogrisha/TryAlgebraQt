#pragma once
#include <MathEditor/include/TextBuffer.h>
#include <Me/include/MeContainer.h>
#include <MathDocumentState.h>
#include <MathEditor/include/Structs.h>

namespace TryAlgebraCore
{
	class MathDocument
	{
	public:
		MathDocument();
		void setDocSize(const glm::vec2& size) { m_doc_size = size; }
		void setText(const std::wstring& str);
		void type(const std::wstring& str);
		void delBackward();
		void delForward();
		void stepLeft();
		void stepRight();
		void updateSelection(const glm::vec2& pos);
		void stopSelection();
		void draw(VisualToolkit* visual_toolkit);
		bool restoreCaretPos(MeBase* me);
		TextBuffer m_text_buffer;
		std::unique_ptr<MeContainer> m_container;
		glm::vec2 m_doc_size;

		bool caret_updated = false;
		bool content_updated = false;
		bool highlight_updated = false;

		bool selecting = false;
		MePath m_caret_pos;

	};
}
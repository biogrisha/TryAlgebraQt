#pragma once
#include <MathEditor/include/TextBuffer.h>
#include <Me/include/MeContainer.h>
#include <MathDocumentState.h>
#include <MathEditor/include/Structs.h>
#include <Helpers/include/EnumFlags.h>

namespace TryAlgebraCore
{
	class MathDocument
	{
		enum class DirtyState : uint8_t
		{
			None = 0,
			Text = 1 << 0,
			Selection = 1 << 1,
			Highlight = 1 << 2
		};

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
	private:
		void markDirty(DirtyState flags);
		void clearDirty();
		void deleteSelected();
		bool hasSelection();
		DirtyState getDirtyState() { return m_dirty_states; }
		TextBuffer m_text_buffer;
		std::unique_ptr<MeContainer> m_container;
		glm::vec2 m_doc_size;

		DirtyState m_dirty_states = DirtyState::None;
		bool m_selecting = false;
		MePath m_selection_start;
		MePath m_selection_end;
		// The drawing function always calculates (if any) one line above and one line below the visible area.
		// so there is always room for a step.
		int m_line_from = 0;
		int m_line_to = 0;
	};

	template<>
	struct enable_bitmask_operators<MathDocument::DirtyState> : std::true_type {};
}
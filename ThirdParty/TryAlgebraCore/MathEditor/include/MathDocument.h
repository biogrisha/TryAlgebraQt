#pragma once
#include <MathEditor/include/TextBuffer.h>
#include <Me/include/MeContainer.h>
#include <MathDocumentState.h>
#include <MathEditor/include/Structs.h>
#include <Helpers/include/EnumFlags.h>
#include <boost/signals2.hpp>
#include <MathEditor/include/DocumentHistory.h>

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
		void setVisualToolkit(const VisualToolkit& visual_toolkit);
		void setDocSize(const glm::vec2& size) { m_doc_size = size; }
		void setText(const std::wstring& str);
		void type(std::wstring str);
		void typeByName(const std::wstring& str);
		void delBackward();
		void delForward();
		void step(StepDir dir, bool with_selection = false);
		void updateSelection(const glm::vec2& pos);
		void stopSelection();
		void selectAll();
		void copy();
		void cut();
		void paste();
		void draw();
		bool restoreCaretPos(MeBase* me);
		void scroll(bool delta);
		void setLineFrom(int line);
		std::wstring getText();
		std::wstring getSelectedText();
		const TextBuffer& textBuffer() const;
		void markDirty();
		int linesCount() const;
		int currentLine() const;
		void undo();
		void redo();
		void goToCaret();
		glm::vec2 caretPos() const;
		boost::signals2::signal<void(int currentLine, int linesCount, int linesOnScreen)> scrollDataChanged;
	private:
		void inverseAction(bool undo);
		void markDirty(DirtyState flags);
		void clearDirty();
		std::wstring deleteSelected();
		bool hasSelection();
		DirtyState getDirtyState() { return m_dirty_states; }
		void adjustLineFrom();
		//clears layout and recalculates center_line - 1 -> cneter_line + 2
		void calcLinesAboveBelow(int center_line);
		bool isLineCalculated(int line_num);
		void filterInput(std::wstring& str);
		TextBuffer m_textBuffer;
		std::unique_ptr<MeContainer> m_container;
		glm::vec2 m_doc_size;

		DirtyState m_dirty_states = DirtyState::None;
		bool m_selecting = false;
		MePath m_selection_start;
		MePath m_selection_end;
		// The drawing function always calculates (if any) one line above and one line below the visible area.
		// so there is always room for a step.
		int m_lineFrom = 0;
		int m_line_to = 0;
		bool m_snap_to_end = false;
		VisualToolkit m_visual_toolkit;
		std::wstring m_copiedText;
		DocumentHistory m_history;
	};

	template<>
	struct enable_bitmask_operators<MathDocument::DirtyState> : std::true_type {};
}
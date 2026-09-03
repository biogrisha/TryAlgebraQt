#include <MathEditor/include/MathDocument.h>
#include <Me/include/MeContainer.h>
#include <Me/include/MeParser.h>
#include <Me/include/MeNewLine.h>
#include <Helpers/include/MeHelpers.h>
#include <FreeTypeWrap.h>
#include <Me/include/MeGlobals.h>
#include <iostream>

namespace TryAlgebraCore
{
	MathDocument::MathDocument()
	{
		m_doc_size = { 0, 0 };
		m_selection_start = {
			LeafPos(0)
		};
		m_selection_end = {
			LeafPos(0)
		};
		m_container = MyRTTI::MakeTypedUnique<MeContainer>();
		m_container->setScalingFactor(1);
	}

	void MathDocument::setVisualToolkit(const VisualToolkit& visual_toolkit)
	{
		m_visual_toolkit = visual_toolkit;
	}

	void MathDocument::setText(const std::wstring& str)
	{
		m_textBuffer.insert(str, 0);
		markDirty(DirtyState::Text);
	}

	void MathDocument::type(std::wstring str)
	{
		bool deletedSelection = false;
		//history info
		std::wstring deletedStr;
		MePath insertPos;

		filterInput(str);
		deletedSelection = hasSelection();
		if (deletedSelection)
		{
			deletedStr = deleteSelected();
		}
		LeafPos& from = std::get<LeafPos>(m_selection_start.back());
		insertPos = m_selection_start;
		m_textBuffer.insert(str, from.pos);
		from.pos += str.size();
		MeHelpers::propagateMeChange(m_selection_start, str.size());
		m_selection_end = m_selection_start;
		adjustLineFrom();
		markDirty(DirtyState::Text | DirtyState::Selection);
		if (deletedSelection)
		{
			m_history.recordReplace(insertPos, str.size(), std::move(deletedStr));
		}
		else
		{
			m_history.recordInsertion(insertPos, str.size());
		}
		m_history.clearRedo();
	}

	void MathDocument::typeByName(const std::wstring& str)
	{
		auto& meTable = MeNames::getMeTable();
		for (auto& me : meTable)
		{
			if (me.first == str)
			{
				type(me.second);
				return;
			}
		}
	}

	void MathDocument::delBackward()
	{
		std::wstring deletedStr;
		if (hasSelection())
		{
			deletedStr = deleteSelected();
		}
		else
		{
			step(StepDir::left, true);
			if (hasSelection())
			{
				deletedStr = deleteSelected();
			}
		}
		m_selection_end = m_selection_start;
		adjustLineFrom();
		markDirty(DirtyState::Text | DirtyState::Selection);
		m_history.recordDeletion(m_selection_start, std::move(deletedStr));
		m_history.clearRedo();
	}

	void MathDocument::delForward()
	{
		std::wstring deletedStr;
		if (hasSelection())
		{
			deletedStr = deleteSelected();
		}
		else
		{
			step(StepDir::right, true);
			if (hasSelection())
			{
				deletedStr = deleteSelected();
			}
		}
		m_selection_end = m_selection_start;
		adjustLineFrom();
		markDirty(DirtyState::Text | DirtyState::Selection);
		m_history.recordDeletion(m_selection_start, std::move(deletedStr));
		m_history.clearRedo();
	}

	void MathDocument::step(StepDir dir, bool with_selection)
	{
		if (m_container->getChildren().empty())
		{
			//document is empty
			return;
		}

		//find line number from selection end
		std::optional<uint64_t> line_num
			= m_textBuffer.getLineNumber(MeHelpers::getPosOrFrom(m_selection_end.front()));

		if (!line_num.has_value())
		{
			return;
		}

		if (!isLineCalculated(line_num.value()))
		{
			calcLinesAboveBelow(line_num.value());
		}
		m_container->step(dir, StepFrom::none, m_selection_end);
		if (!with_selection)
		{
			m_selection_start = m_selection_end;
		}
		line_num = m_textBuffer.getLineNumber(MeHelpers::getPosOrFrom(m_selection_end.front()));
		if (dir == StepDir::right || dir == StepDir::down)
		{
			if (m_line_to == line_num)
			{
				++m_lineFrom;
				m_snap_to_end = true;
				markDirty(DirtyState::Text);
			}
			else if (line_num == m_line_to - 1 && !m_snap_to_end)
			{
				m_snap_to_end = true;
				markDirty(DirtyState::Text);
			}
		}
		else
		{
			if (m_lineFrom - 1 == line_num)
			{
				--m_lineFrom;
				m_snap_to_end = false;
				markDirty(DirtyState::Text);
			}
			else if (line_num == m_lineFrom && m_snap_to_end)
			{
				m_snap_to_end = false;
				markDirty(DirtyState::Text);
			}
		}
		markDirty(DirtyState::Selection);
	}

	void MathDocument::updateSelection(const glm::vec2& pos)
	{
		if (m_container->getChildren().empty())
		{
			return;
		}
		if (!m_selecting)
		{
			m_selecting = true;
			MePath selection_start;
			if (MeHelpers::getPathAtPos(m_container.get(), pos, selection_start))
			{
				m_selection_start = std::move(selection_start);
			}
		}
		MePath selection_end;
		if (MeHelpers::getPathAtPos(m_container.get(), pos, selection_end))
		{
			m_selection_end = std::move(selection_end);
		}
		markDirty(DirtyState::Selection);
	}

	void MathDocument::stopSelection()
	{
		m_selecting = false;
	}

	void MathDocument::selectAll()
	{
		m_selection_start.clear();
		m_selection_start.push_back(LeafPos{ 0 });
		m_selection_end.clear();
		m_selection_end.push_back(LeafPos{ m_textBuffer.getSize() });
		markDirty(DirtyState::Selection);
	}

	void MathDocument::copy()
	{
		m_copiedText = getSelectedText();
	}

	void MathDocument::cut()
	{
		copy();
		if (hasSelection())
		{
			deleteSelected();
		}
		m_selection_end = m_selection_start;
		adjustLineFrom();
		markDirty(DirtyState::Text | DirtyState::Selection);
	}

	void MathDocument::paste()
	{
		type(m_copiedText);
	}

	void MathDocument::draw()
	{
		std::lock_guard<std::mutex> guard(m_visual_toolkit.mdocState->mtx());
		if (hasFlag(getDirtyState(), DirtyState::Text))
		{
			m_visual_toolkit.mdocState->at(1).clear();
		}
		if (hasFlag(getDirtyState(), DirtyState::Selection))
		{
			m_visual_toolkit.mdocState->at(0).clear();
		}

		if (hasFlag(getDirtyState(), DirtyState::Text))
		{
			float line_before_h = 0;
			float cont_visible_y = 0;
			m_container = MyRTTI::MakeTypedUnique<MeContainer>();
			m_container->setScalingFactor(1);
			if (m_lineFrom > 0)
			{
				//calculate one line above
				MeParser parser(m_textBuffer, std::max(0, m_lineFrom - 1));
				parser.parseLine(m_container.get());
				m_container->calcLine(&m_visual_toolkit);
				line_before_h = m_container->getSize().y;
			}
			MeParser parser(m_textBuffer, std::max(0, m_lineFrom));
			m_line_to = m_lineFrom;
			while (true)
			{
				++m_line_to;
				if (!parser.parseLine(m_container.get()))
				{
					//end of document
					break;
				}
				m_container->calcLine(&m_visual_toolkit);
				cont_visible_y = m_container->getSize().y - line_before_h;
				if (cont_visible_y > m_doc_size.y)
				{
					//exceeded document size
					//calculate one more line
					if (parser.parseLine(m_container.get()))
					{
						m_container->calcLine(&m_visual_toolkit);
					}
					break;
				}
			}

			m_container->setPosY(-line_before_h - m_snap_to_end * (cont_visible_y - m_doc_size.y));
			m_container->calculatePos();
			m_container->setSizeX(std::max(m_doc_size.x, m_container->getSize().x));
			m_container->setSizeY(std::max(m_doc_size.y, m_container->getSize().y));
			restoreCaretPos(m_container.get());
			m_container->draw(&m_visual_toolkit);
		}
		if (hasFlag(getDirtyState(), DirtyState::Selection))
		{
			//draw caret
			auto caret_data = MeHelpers::getCaretData(m_container.get(), m_selection_end);

			FSpriteInstByName sprite;
			sprite.SpriteName = "Caret.png";
			sprite.Pos = caret_data.Pos;
			sprite.Size = caret_data.Size;
			sprite.Alpha = 1;
			m_visual_toolkit.mdocState->at(0).addSprite(sprite);
			if (!m_container->getChildren().empty())
			{
				MeHelpers::highlightSelected(m_container.get(), m_selection_start, m_selection_end, &m_visual_toolkit);
			}
		}

		clearDirty();
		scrollDataChanged(m_lineFrom, textBuffer().getLinesCount(), linesCount());
	}

	bool MathDocument::restoreCaretPos(MeBase* me)
	{
		return false;
	}

	void MathDocument::scroll(bool up)
	{
		auto oldLine = m_lineFrom;
		m_lineFrom += -up + !up;
		m_lineFrom = std::clamp<int>(m_lineFrom, 0, m_textBuffer.getLinesCount() - 1);
		if (m_lineFrom != oldLine)
		{
			m_snap_to_end = false;
			markDirty(DirtyState::Selection | DirtyState::Text);
		}
	}

	void MathDocument::setLineFrom(int line)
	{

		line = std::clamp<int>(line, 0, m_textBuffer.getLinesCount() - 1);
		if (line == m_lineFrom)
		{
			return;
		}
		m_lineFrom = line;
		m_snap_to_end = false;
		markDirty(DirtyState::Selection | DirtyState::Text);
	}

	std::wstring MathDocument::getText()
	{
		return m_textBuffer.getBuff();
	}

	std::wstring MathDocument::getSelectedText()
	{
		auto selectionStart = m_selection_start;
		auto selectionEnd = m_selection_end;
		MeHelpers::orderPaths(selectionStart, selectionEnd);
		MeHelpers::trimToCommonContainer(selectionStart, selectionEnd);
		int from = std::get<LeafPos>(selectionStart.back()).pos;
		int to = std::get<LeafPos>(selectionEnd.back()).pos;
		return m_textBuffer.getSubstring(from, to);
	}

	const TextBuffer& MathDocument::textBuffer() const
	{
		return m_textBuffer;
	}

	void MathDocument::markDirty()
	{
		markDirty(DirtyState::Selection | DirtyState::Text);
	}

	int MathDocument::linesCount() const
	{
		return m_line_to - m_lineFrom;
	}

	int MathDocument::currentLine() const
	{
		return m_lineFrom;
	}

	void TryAlgebraCore::MathDocument::undo()
	{
		inverseAction(true);
	}

	void TryAlgebraCore::MathDocument::redo()
	{
		inverseAction(false);
	}

	void MathDocument::goToCaret()
	{
		int from = std::get<LeafPos>(m_selection_end.back()).pos;
		uint64_t line = m_textBuffer.getLineNumber(from).value();
		if (line < m_lineFrom || line >= m_line_to)
		{
			m_lineFrom = line;
		}
		markDirty(DirtyState::Selection | DirtyState::Text);
	}

	void MathDocument::inverseAction(bool undo)
	{
		auto actionOpt = m_history.pop(undo);
		if (!actionOpt)
		{
			return;
		}
		if (undo)
		{
			m_history.setUndoMode(false);
		}

		std::visit([this](auto&& action) {
			using T = std::decay_t<decltype(action)>;

			if constexpr (std::is_same_v<T, InvertDelete>) {
				m_selection_start = action.pos;
				m_selection_end = action.pos;
				std::get<LeafPos>(m_selection_start.back()).pos += action.string.size();
				std::get<LeafPos>(m_selection_end.back()).pos += action.string.size();

				m_selecting = false;
				int from = std::get<LeafPos>(action.pos.back()).pos;
				m_textBuffer.insert(action.string, from);
				markDirty(DirtyState::Selection | DirtyState::Text);
				m_history.recordInsertion(action.pos, action.string.size());
			}
			else if constexpr (std::is_same_v<T, InvertInsert>) {
				m_selection_start = action.pos;
				m_selection_end = action.pos;
				m_selecting = false;
				int from = std::get<LeafPos>(action.pos.back()).pos;
				std::wstring deletedStr = m_textBuffer.getSubstring(from, from + action.size);
				m_textBuffer.del(from, from + action.size);
				markDirty(DirtyState::Selection | DirtyState::Text);
				m_history.recordDeletion(action.pos, deletedStr);
			}
			else if constexpr (std::is_same_v<T, InvertReplace>) {
				m_selection_start = action.pos;
				m_selection_end = action.pos;
				std::get<LeafPos>(m_selection_start.back()).pos += action.deletedStr.size();
				std::get<LeafPos>(m_selection_end.back()).pos += action.deletedStr.size();
				m_selecting = false;
				int from = std::get<LeafPos>(action.pos.back()).pos;
				std::wstring deletedStr = m_textBuffer.getSubstring(from, from + action.insertSize);
				m_textBuffer.del(from, from + action.insertSize);
				m_textBuffer.insert(action.deletedStr, from);
				markDirty(DirtyState::Selection | DirtyState::Text);
				m_history.recordReplace(action.pos, action.deletedStr.size(), deletedStr);
			}
			}, actionOpt.value());
		if (undo)
		{
			m_history.setUndoMode(true);
		}
	}

	void MathDocument::markDirty(DirtyState flags)
	{
		m_dirty_states |= flags;
	}

	void MathDocument::clearDirty()
	{
		m_dirty_states = DirtyState::None;
	}

	std::wstring MathDocument::deleteSelected()
	{
		MeHelpers::orderPaths(m_selection_start, m_selection_end);
		MeHelpers::trimToCommonContainer(m_selection_start, m_selection_end);
		int from = std::get<LeafPos>(m_selection_start.back()).pos;
		int to = std::get<LeafPos>(m_selection_end.back()).pos;
		std::wstring deletedString = m_textBuffer.getSubstring(from, to);
		m_textBuffer.del(from, to);
		MeHelpers::propagateMeChange(m_selection_start, from - to);
		return deletedString;
	}

	bool MathDocument::hasSelection()
	{
		return m_selection_end != m_selection_start;
	}

	void MathDocument::adjustLineFrom()
	{
		std::optional<uint64_t> line_num = m_textBuffer.getLineNumber(std::get<LeafPos>(m_selection_end.back()).pos);
		if (line_num.has_value())
		{
			if (line_num >= m_line_to || line_num < m_lineFrom)
			{
				m_lineFrom = line_num.value();
			}
		}

	}

	void MathDocument::calcLinesAboveBelow(int center_line)
	{
		// estimate from-to lines
		m_lineFrom = std::max(0, center_line - 1);
		size_t to = std::min<size_t>(m_textBuffer.getLinesCount(), center_line + 2);

		//clear container
		m_container = MyRTTI::MakeTypedUnique<MeContainer>();
		MeParser parser(m_textBuffer, m_lineFrom);

		//parse and calculate from-to lines
		for (size_t i = m_lineFrom; i < to; ++i)
		{
			parser.parseLine(m_container.get());
			m_container->calcLine(&m_visual_toolkit);
		}
		m_container->calculatePos();
		markDirty(DirtyState::Selection | DirtyState::Text);
	}

	bool MathDocument::isLineCalculated(int line_num)
	{
		//note, that canvas always calculates line above and below if it can
		return line_num <= m_line_to && line_num >= m_lineFrom - 1;
	}

	void MathDocument::filterInput(std::wstring& str)
	{
		if (m_selection_start.size() > 1)
		{
			if (str == L"\n")
			{
				str = L"\\" + MeNames::new_line + L"\\\\";
			}
		}
	}
}

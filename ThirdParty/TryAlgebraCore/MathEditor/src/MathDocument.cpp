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
	}

	void MathDocument::setVisualToolkit(const VisualToolkit& visual_toolkit)
	{
		m_visual_toolkit = visual_toolkit;
	}

	void MathDocument::setText(const std::wstring& str)
	{
		m_text_buffer.insert(str, 0);
		markDirty(DirtyState::Text);
	}

	void MathDocument::type(const std::wstring& str)
	{
		auto str_copy = str;
		filterInput(str_copy);
		if (hasSelection())
		{
			deleteSelected();
		}
		LeafPos& from = std::get<LeafPos>(m_selection_start.back());
		m_text_buffer.insert(str_copy, from.pos);
		from.pos += str_copy.size();
		MeHelpers::propagateMeChange(m_selection_start, str_copy.size());
		m_selection_end = m_selection_start;
		adjustLineFrom();
		markDirty(DirtyState::Text | DirtyState::Selection);
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
		if (hasSelection())
		{
			deleteSelected();
		}
		else
		{
			step(StepDir::left, true);
			if (hasSelection())
			{
				deleteSelected();
			}
		}
		m_selection_end = m_selection_start;
		adjustLineFrom();
		markDirty(DirtyState::Text | DirtyState::Selection);
	}

	void MathDocument::delForward()
	{
		if (hasSelection())
		{
			deleteSelected();
		}
		else
		{
			step(StepDir::right, true);
			if (hasSelection())
			{
				deleteSelected();
			}
		}
		m_selection_end = m_selection_start;
		adjustLineFrom();
		markDirty(DirtyState::Text | DirtyState::Selection);
	}

	void MathDocument::step(StepDir dir, bool with_selection)
	{
		if (m_container->getChildren().empty())
		{
			return;
		}
		std::optional<uint64_t> line_num = m_text_buffer.getLineNumber(MeHelpers::getPosOrFrom(m_selection_end.front()));
		if (!line_num.has_value())
		{
			return;
		}
		if (isLineOutside(line_num.value()))
		{
			calcLinesAboveBelow(line_num.value());
		}
		m_container->step(dir, StepFrom::none, m_selection_end);
		if (!with_selection)
		{
			m_selection_start = m_selection_end;
		}
		line_num = m_text_buffer.getLineNumber(MeHelpers::getPosOrFrom(m_selection_end.front()));
		if (dir == StepDir::right || dir == StepDir::down)
		{
			if (m_line_to == line_num)
			{
				++m_line_from;
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
			if (m_line_from - 1 == line_num)
			{
				--m_line_from;
				m_snap_to_end = false;
				markDirty(DirtyState::Text);
			}
			else if (line_num == m_line_from && m_snap_to_end)
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

	void MathDocument::draw()
	{
		if(hasFlag(getDirtyState(), DirtyState::Text))
		{
			float line_before_h = 0;
			float cont_visible_y = 0;
			m_visual_toolkit.mdocState->Clear(true, false);
			m_container = MyRTTI::MakeTypedUnique<MeContainer>();
			m_container->setScalingFactor(1);
			if (m_line_from > 0)
			{
				//calculate one line above
				MeParser parser(m_text_buffer, std::max(0, m_line_from - 1));
				parser.parseLine(m_container.get());
				m_container->calcLine(&m_visual_toolkit);
				line_before_h = m_container->getSize().y;
			}
			MeParser parser(m_text_buffer, std::max(0, m_line_from));
			m_line_to = m_line_from;
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
				if (m_container->getSize().y - line_before_h > m_doc_size.y)
				{
					//exceeded document size
					//calculate one more line
					if(parser.parseLine(m_container.get()))
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
		if(hasFlag(getDirtyState(), DirtyState::Selection))
		{
			m_visual_toolkit.mdocState->Clear(false, true);
			//draw caret
			auto caret_data = MeHelpers::getCaretData(m_container.get(), m_selection_end);
			m_visual_toolkit.mdocState->SetCaret(caret_data);
			if(!m_container->getChildren().empty())
			{
				MeHelpers::highlightSelected(m_container.get(), m_selection_start, m_selection_end, &m_visual_toolkit);
			}
		}
		clearDirty();
	}
	bool MathDocument::restoreCaretPos(MeBase* me)
	{
		return false;
	}
	void MathDocument::scroll(bool up)
	{
		m_line_from += -up + !up;
		m_line_from = std::clamp<int>(m_line_from, 0, m_text_buffer.getLinesCount() - 1);
		m_snap_to_end = false;
		markDirty(DirtyState::Selection | DirtyState::Text);
	}
	std::wstring MathDocument::getText()
	{
		return m_text_buffer.getBuff();
	}
	void MathDocument::markDirty(DirtyState flags)
	{
		m_dirty_states |= flags;
	}
	void MathDocument::clearDirty()
	{
		m_dirty_states = DirtyState::None;
	}
	void MathDocument::deleteSelected()
	{
		MeHelpers::orderPaths(m_selection_start, m_selection_end);
		MeHelpers::trimToCommonContainer(m_selection_start, m_selection_end);
		int from = std::get<LeafPos>(m_selection_start.back()).pos;
		int to = std::get<LeafPos>(m_selection_end.back()).pos;
		m_text_buffer.del(from, to);
		MeHelpers::propagateMeChange(m_selection_start, from - to);
	}
	bool MathDocument::hasSelection()
	{
		return m_selection_end != m_selection_start;
	}
	void MathDocument::adjustLineFrom()
	{
		std::optional<uint64_t> line_num = m_text_buffer.getLineNumber(std::get<LeafPos>(m_selection_end.back()).pos);
		if (line_num.has_value())
		{
			if(line_num >= m_line_to || line_num < m_line_from)
			{
				m_line_from = line_num.value();
			}
		}
		
	}
	void MathDocument::calcLinesAboveBelow(int center_line)
	{
		m_line_from = std::max(0, center_line - 1);
		size_t to = std::min<size_t>(m_text_buffer.getLinesCount(), center_line + 2);
		m_container = MyRTTI::MakeTypedUnique<MeContainer>();
		MeParser parser(m_text_buffer, m_line_from);
		
		for (size_t i = m_line_from; i < to; ++i)
		{
			parser.parseLine(m_container.get());
			m_container->calcLine(&m_visual_toolkit);
		}
		m_container->calculatePos();
		markDirty(DirtyState::Selection | DirtyState::Text);
	}
	bool MathDocument::isLineOutside(int line_num)
	{
		return line_num > m_line_to || line_num < m_line_from - 1;
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

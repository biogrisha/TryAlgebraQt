#include <MathEditor/include/MathDocument.h>
#include <Me/include/MeContainer.h>
#include <Me/include/MeParser.h>
#include <Me/include/MeNewLine.h>
#include <Helpers/include/MeHelpers.h>
#include <FreeTypeWrap.h>
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

	void MathDocument::setText(const std::wstring& str)
	{
		m_text_buffer.insert(str, 0);
		markDirty(DirtyState::Text);
	}

	void MathDocument::type(const std::wstring& str)
	{
		if (hasSelection())
		{
			deleteSelected();
		}
		LeafPos& from = std::get<LeafPos>(m_selection_start.back());
		m_text_buffer.insert(str, from.pos);
		from.pos += str.size();
		m_selection_end = m_selection_start;
		markDirty(DirtyState::Text | DirtyState::Selection);
	}

	void MathDocument::delBackward()
	{
		if (hasSelection())
		{
			deleteSelected();
		}
		else
		{
			//at 0 -> no grandparent->do nothing
			//at 0 -> delete grandparent-> caret before
			// not 0 -> in container -> delete
			// not 0 -> not in container -> 

		}
		m_selection_end = m_selection_start;
		markDirty(DirtyState::Text | DirtyState::Selection);
	}

	void MathDocument::delForward()
	{

	}

	void MathDocument::stepLeft()
	{
		if (m_container->getChildren().empty())
		{
			return;
		}
		m_container->step(StepDir::left, StepFrom::none, m_selection_end);
		m_selection_start = m_selection_end;
		if (m_line_from - 1 == m_text_buffer.getLineNumber(std::get<LeafPos>(m_selection_start.back()).pos))
		{
			--m_line_from;
			markDirty(DirtyState::Text);
		}
		markDirty(DirtyState::Selection);
	}

	void MathDocument::stepRight()
	{
		if (m_container->getChildren().empty())
		{
			return;
		}
		m_container->step(StepDir::right, StepFrom::none, m_selection_end);
		m_selection_start = m_selection_end;
		if (m_line_to == m_text_buffer.getLineNumber(std::get<LeafPos>(m_selection_start.back()).pos))
		{
			++m_line_from;
			markDirty(DirtyState::Text);
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

	void MathDocument::draw(VisualToolkit* visual_toolkit)
	{
		if(hasFlag(getDirtyState(), DirtyState::Text))
		{
			visual_toolkit->mdoc_state->Clear(true, false);
			m_container = MyRTTI::MakeTypedUnique<MeContainer>();
			m_container->setScalingFactor(1);
			if (m_line_from > 0)
			{
				//calculate one line above
				MeParser parser(m_text_buffer, std::max(0, m_line_from - 1));
				parser.parseLine(m_container.get());
				m_container->calcLine(visual_toolkit);
				m_container->setPos({ 0, -m_container->getSize().y });
			}
			MeParser parser(m_text_buffer, std::max(0, m_line_from));
			m_line_to = m_line_from;
			while (true)
			{
				if (!parser.parseLine(m_container.get()))
				{
					//end of document
					break;
				}
				m_container->calcLine(visual_toolkit);
				++m_line_to;
				if (m_container->getSize().y > m_doc_size.y)
				{
					//exceeded document size
					//calculate one more line
					m_container->calcLine(visual_toolkit);
					break;
				}
			}
			m_container->calculatePos();
			m_container->setSize(m_doc_size);
			restoreCaretPos(m_container.get());
			m_container->draw(visual_toolkit);
		}
		if(hasFlag(getDirtyState(), DirtyState::Selection))
		{
			visual_toolkit->mdoc_state->Clear(false, true);
			//draw caret
			auto caret_data = MeHelpers::getCaretData(m_container.get(), m_selection_end);
			visual_toolkit->mdoc_state->SetCaret(caret_data);
			MeHelpers::highlightSelected(m_container.get(), m_selection_start, m_selection_end, visual_toolkit);
		}

	}
	bool MathDocument::restoreCaretPos(MeBase* me)
	{
		return false;
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
		size_t from = std::get<LeafPos>(m_selection_start.back()).pos;
		size_t to = std::get<LeafPos>(m_selection_end.back()).pos;
		m_text_buffer.del(from, to);
	}
	bool MathDocument::hasSelection()
	{
		return m_selection_end != m_selection_start;
	}
}

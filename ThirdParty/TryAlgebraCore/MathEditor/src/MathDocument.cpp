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
		content_updated = true;
	}

	void MathDocument::type(const std::wstring& str)
	{
		MeHelpers::orderPaths(m_selection_start, m_selection_end);
		MeHelpers::trimToCommonContainer(m_selection_start, m_selection_end);
		size_t from = std::get<LeafPos>(m_selection_start.back()).pos;
		size_t to = std::get<LeafPos>(m_selection_end.back()).pos;
		m_text_buffer.del(from, to);
		m_text_buffer.insert(str, from);
		std::get<LeafPos>(m_selection_start.back()).pos += str.size();
		m_selection_end = m_selection_start;
		content_updated = true;
		caret_updated = true;
		highlight_updated = true;
	}

	void MathDocument::delBackward()
	{

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
		caret_updated = true;
		highlight_updated = true;
	}

	void MathDocument::stepRight()
	{
		if (m_container->getChildren().empty())
		{
			return;
		}
		m_container->step(StepDir::right, StepFrom::none, m_selection_end);
		m_selection_start = m_selection_end;
		caret_updated = true;
		highlight_updated = true;
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
		caret_updated = true;
		highlight_updated = true;
	}

	void MathDocument::stopSelection()
	{
		m_selecting = false;
	}

	void MathDocument::draw(VisualToolkit* visual_toolkit)
	{
		highlight_updated |= content_updated;
		caret_updated |= content_updated;
		if(content_updated)
		{
			m_container = MyRTTI::MakeTypedUnique<MeContainer>();
			MeParser parser(m_text_buffer, 0);
			while (true)
			{
				if (!parser.parseLine(m_container.get()))
				{
					if (m_container->getChildren().empty() || MyRTTI::Is<MeNewLine>(m_container->getChildren().back().get()))
					{
						m_container->addEmptyLine();
					}
					break;
				}
				m_container->setScalingFactor(1);
				m_container->calcLine(visual_toolkit);
				if (m_container->getSize().y > m_doc_size.y)
				{
					break;
				}
			}
			m_container->calculatePos();
			m_container->setSize(m_doc_size);
			restoreCaretPos(m_container.get());
			m_container->draw(visual_toolkit);
		}
		if(caret_updated)
		{
			//draw caret
			auto caret_data = MeHelpers::getCaretData(m_container.get(), m_selection_end);
			visual_toolkit->mdoc_state->SetCaret(caret_data);
		}
		if (highlight_updated)
		{
			visual_toolkit->mdoc_state->Clear(false, true);
			MeHelpers::highlightSelected(m_container.get(), m_selection_start, m_selection_end, visual_toolkit);
		}

		content_updated = false;
		caret_updated = false;
		highlight_updated = false;
	}
	bool MathDocument::restoreCaretPos(MeBase* me)
	{
		return false;
	}
}

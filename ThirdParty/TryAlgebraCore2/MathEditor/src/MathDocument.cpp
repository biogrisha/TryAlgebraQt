#include <MathEditor/include/MathDocument.h>
#include <Me/include/MeContainer.h>
#include <Me/include/MeParser.h>
#include <Me/include/MeNewLine.h>
#include <Helpers/include/MeHelpers.h>
#include <FreeTypeWrap.h>
#include <iostream>

namespace TryAlgebraCore2
{
	MathDocument::MathDocument()
	{
		m_doc_size = { 0, 0 };
	}

	void MathDocument::setText(const std::wstring& str)
	{
		m_text_buffer.insert(str, 0);
		content_updated = true;
	}

	void MathDocument::type(const std::wstring& str)
	{

	}

	void MathDocument::delBackward()
	{

	}

	void MathDocument::delForward()
	{

	}

	void MathDocument::stepLeft()
	{
		caret_updated = true;
	}

	void MathDocument::stepRight()
	{
		caret_updated = true;
	}

	void MathDocument::updateSelection(const glm::vec2& pos)
	{
		
	}

	void MathDocument::stopSelection()
	{
		selecting = false;
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
				if (m_container->getSizeY() > m_doc_size.y)
				{
					break;
				}
			}
			m_container->setSize(m_doc_size);
			restoreCaretPos(m_container.get());
			m_container->draw(visual_toolkit);
		}
		if(caret_updated)
		{
			//draw caret
			//auto caret_data = MeHelpers::getCaretData(m_container.get(), m_caret_pos);
			//visual_toolkit->mdoc_state->SetCaret(caret_data);
		}
		if (highlight_updated)
		{
			//visual_toolkit->mdoc_state->AddRect
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

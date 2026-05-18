#include <Me/include/MeParser.h>
#include <Me/include/MeCharacter.h>
#include <Me/include/MeContainer.h>
#include <Me/include/MeNewLine.h>

namespace TryAlgebraCore2
{
	std::unique_ptr<MeBase> MeGenerator::generateMe(const std::wstring& me_str)
	{
		return std::unique_ptr<MeBase>();
	}
	MeParser::MeParser(const TextBuffer& text_buffer, int line_num)
		:m_it(text_buffer, line_num)
	{

	}

	bool MeParser::parseLine(MeContainer* container)
	{
		m_parent = container;
		if (m_it.isEnd())
		{
			return false;
		}
		parse(true);
		return true;
	}

	void MeParser::parse(bool parse_one_line)
	{
		m_parent->setChTo(m_it.getChId());
		while (true)
		{
			if (m_it.isEnd())
			{
				return;
			}
			wchar_t ch = m_it.next();
			if (ch == L'\n')
			{
				auto me = MyRTTI::MakeTypedUnique<MeNewLine>();
				me->setParent(m_parent);
				me->setChFrom(m_it.getChId() - 1);
				me->setChTo(m_it.getChId());
				m_parent->addChild(std::move(me));
				if (parse_one_line)
				{
					return;
				}
			}
			else if (ch == '\\')
			{
				consumeMe();
			}
			else
			{
				auto me = MyRTTI::MakeTypedUnique<MeCharacter>(ch);
				me->setParent(m_parent);
				me->setChFrom(m_it.getChId() - 1);
				me->setChTo(m_it.getChId());
				m_parent->addChild(std::move(me));
			}
		}
	}

	void MeParser::consumeMe()
	{
		std::wstring str;
		while (true)
		{
			wchar_t ch = m_it.next();
			if (ch == L'\\')
			{
				consumeMeta();
			}
		}
	}

	void MeParser::consumeMeta()
	{
		std::wstring str;
		while (true)
		{
			wchar_t ch = m_it.next();
			if (ch == L'{')
			{
				m_current->setMeta(str);
				startChildren();
				return;
			}
			str += ch;
		}
	}

	void MeParser::startChildren()
	{
		MeBase* parent = m_parent;
		MeBase* current = m_current;
		while(m_it.current() != '\}')
		{
			auto cont = MyRTTI::MakeTypedUnique<MeContainer>();
			m_parent = cont.get();
			current->addChild(std::move(cont));
			parse(false);
		}
		m_parent = parent;
	}

}
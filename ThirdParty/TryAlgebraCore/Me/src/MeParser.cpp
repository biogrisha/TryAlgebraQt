#include <Me/include/MeParser.h>
#include <Me/include/MeCharacter.h>
#include <Me/include/MeContainer.h>
#include <Me/include/MeNewLine.h>
#include <Me/include/MeGlobals.h>
#include <Me/include/MeFromTo.h>

namespace TryAlgebraCore
{
	MeParser::MeParser(const TextBuffer& text_buffer, int line_num)
		:m_it(text_buffer, line_num)
	{
		m_factory.emplace(MeNames::from_to,
			[]()
			{
				return MyRTTI::MakeTypedUnique<MeFromTo>();
			});
		m_factory.emplace(MeNames::new_line,
			[]()
			{
				return MyRTTI::MakeTypedUnique<MeNewLine>();
			});
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

	MeParser::ParsingResult MeParser::parse(bool parse_one_line)
	{
		m_parent->setChTo(m_it.getChId());
		while (true)
		{
			if (m_it.isEnd())
			{
				return ParsingResult::end;
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
					return ParsingResult::end_line;
				}
			}
			else if (ch == '\\')
			{
				if (auto next_ch = m_it.lookAhead(0))
				{
					if (*next_ch == L',')
					{
						//next child
						//go up the stack
						m_it.next();
						return ParsingResult::next_child;
					}
					else if (*next_ch == L'}')
					{
						//end children
						// go up the stack
						m_it.next();
						return ParsingResult::end_children;
					}
				}
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
		std::wstring me_name;
		size_t ch_from = m_it.getChId() - 1;
		while (true)
		{
			wchar_t ch = m_it.next();
			if (ch == L'\\')
			{
				auto new_me = make(me_name);
				assert(new_me);
				m_current = new_me.get();
				m_parent->addChild(std::move(new_me));
				m_current->setChFrom(ch_from);
				m_current->setParent(m_parent);
				if (auto next_ch = m_it.lookAhead(0))
				{
					if (*next_ch == L'{')
					{
						m_it.next();
						startChildren();
					}
					else if (*next_ch == L'\\')
					{
						m_it.next();
					}
					else
					{
						consumeMeta();
					}
				}
				m_current->setChTo(m_it.getChId());
				return;
			}
			me_name += ch;
		}
	}

	void MeParser::consumeMeta()
	{
		std::wstring str;
		while (true)
		{
			wchar_t ch = m_it.next();
			if (ch == L'\\')
			{
				if (auto next = m_it.lookAhead(0))
				{
					if (*next == L'{')
					{
						m_current->setMeta(str);
						m_it.next();
						startChildren();
						return;
					}
				}
			}
			str += ch;
		}
	}

	void MeParser::startChildren()
	{
		MeBase* parent = m_parent;
		MeBase* current = m_current;
		while(true)
		{
			auto cont = MyRTTI::MakeTypedUnique<MeContainer>();
			cont->setChFrom(m_it.getChId());
			cont->setParent(current);
			m_parent = cont.get();
			current->addChild(std::move(cont));
			if (parse(false) == ParsingResult::end_children)
			{
				break;
			}
			
		}
		m_current = current;
		m_parent = parent;
	}

	std::unique_ptr<MeBase> MeParser::make(const std::wstring& name)
	{
		auto found = m_factory.find(name);
		if (found != m_factory.end())
		{
			return found->second();
		}
		return std::unique_ptr<MeBase>();
	}

}

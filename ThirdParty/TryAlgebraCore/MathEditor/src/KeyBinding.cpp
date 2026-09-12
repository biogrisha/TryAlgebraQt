#include <MathEditor/include/KeyBinding.h>

namespace TryAlgebraCore
{
	void KeyBinding::setConfiguration(const std::wstring& config)
	{
		m_bindings.clear();
		std::wstring left;
		std::wstring right;
		bool readLeft = true;
		for (wchar_t ch : config)
		{
			if (ch == L'\n')
			{
				if (!left.empty() && !right.empty())
				{
					m_bindings.emplace(left, right);
				}
				left.clear();
				right.clear();
				readLeft = true;
				continue;
			}
			if (ch == L':')
			{
				readLeft = false;
				continue;
			}

			if (readLeft)
			{
				left += ch;
			}
			else
			{
				right += ch;
			}
		}

		if (!left.empty() && !right.empty())
		{
			m_bindings.emplace(left, right);
		}
	}

	void KeyBinding::addKey(wchar_t ch)
	{
		m_currentCommand += ch;
	}

	std::optional<std::wstring> KeyBinding::runCommand()
	{
		auto found = m_bindings.find(m_currentCommand);
		m_currentCommand.clear();
		if (found != m_bindings.end())
		{
			return found->second;
		}
		return std::nullopt;
	}
}
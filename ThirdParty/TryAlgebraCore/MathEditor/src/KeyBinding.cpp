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
}
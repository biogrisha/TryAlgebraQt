#pragma once
#include <string>
#include <unordered_map>

namespace TryAlgebraCore
{
	class KeyBinding
	{
	public:
		void setConfiguration(const std::wstring& config);
	private:
		std::unordered_map<std::wstring, std::wstring> m_bindings;
	};
}
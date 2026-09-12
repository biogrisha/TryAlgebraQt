#pragma once
#include <string>
#include <unordered_map>
#include <optional>

namespace TryAlgebraCore
{
	class KeyBinding
	{
	public:
		void setConfiguration(const std::wstring& config);
		void addKey(wchar_t ch);
		std::optional<std::wstring> runCommand();

	private:
		std::unordered_map<std::wstring, std::wstring> m_bindings;
		std::wstring m_currentCommand;
	};
}
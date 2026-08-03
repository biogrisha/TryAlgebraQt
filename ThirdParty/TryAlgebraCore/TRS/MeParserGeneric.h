#pragma once
#include <string>
#include <functional>

namespace TryAlgebraCore::Trs
{

	class MeParserGeneric
	{
		enum class State
		{
			ReadName,
			ReadMeta,
			ReadGlyph,
			None
		};
	public:
		MeParserGeneric(const std::wstring& str);
		void parse();
		std::function<void(const std::wstring_view& str)> createMe;
		std::function<void(const std::wstring_view& str)> addMeta;
		std::function<void(wchar_t)> addGlyph;
		std::function<void()> startChildren;
		std::function<void()> endChildren;
		std::function<void()> nextChild;
	private:
		const std::wstring& m_str;
	};

}
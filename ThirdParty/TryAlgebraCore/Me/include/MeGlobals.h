#pragma once
#include <string>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
namespace TryAlgebraCore
{
	inline uint32_t g_min_font_size = 5;
	inline uint32_t g_font_def_height = 20;
	inline glm::vec2 g_caret_def_size = { 2, 20 };
	inline glm::vec2 g_invalid_caret_pos = { -100, -100 };

	namespace MeNames
	{
		inline std::wstring from_to = L"ft";
		inline std::wstring integral = L"∫";
		inline std::wstring integral2 = L"∬";
		inline std::wstring integral3 = L"∭";

		inline std::wstring new_line = L"nl";

		inline std::wstring cont = L"ct";

		inline std::wstring variable = L"vr";
		inline std::wstring variableMulti = L"mlt";
		inline std::wstring variableUni = L"uni";


		inline std::wstring term = L"trm";
		inline std::wstring termFunction = L"f";
		inline std::wstring termToken = L"tok";

		inline std::wstring power = L"pw";

		inline std::wstring fraction = L"fr";

		inline std::wstring makeMe(const std::wstring& name, const std::wstring& meta, const size_t cont_num)
		{
			std::wstring res;
			if (meta.empty())
			{
				res = L"\\" + name + L"\\{";
			}
			else
			{
				res = L"\\" + name + L"\\" + meta + L"\\{";
			}
			for (size_t i = 0; i < cont_num - 1; ++i)
			{
				res += L"\\,";
			}
			res += L"\\}";
			return res;
		}

		inline std::vector<std::pair<std::wstring, std::wstring>>& getMeTable()
		{
			static std::vector<std::pair<std::wstring, std::wstring>> res;
			if (res.empty())
			{
				res.emplace_back(L"Integral", makeMe(from_to, integral, 2));
				res.emplace_back(L"Double integral", makeMe(from_to, integral2, 2));
				res.emplace_back(L"Triple integral", makeMe(from_to, integral3, 2));
				res.emplace_back(L"Multi variable", makeMe(variable, variableMulti, 1));
				res.emplace_back(L"Uni variable", makeMe(variable, variableUni, 1));
				res.emplace_back(L"For all", L"∀");
				res.emplace_back(L"Exists", L"∃");
				res.emplace_back(L"Term2", makeMe(term, termFunction, 3));
				res.emplace_back(L"Term1", makeMe(term, termFunction, 2));
				res.emplace_back(L"Token", makeMe(term, termToken, 1));
				res.emplace_back(L"Power", makeMe(power, L"", 1));
				res.emplace_back(L"Fraction", makeMe(fraction, L"", 2));
			}
			return res;
		}
	}
}
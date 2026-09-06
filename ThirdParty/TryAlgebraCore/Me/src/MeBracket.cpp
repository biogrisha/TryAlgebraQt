#include <Me/include/MeBracket.h>

namespace TryAlgebraCore
{

	namespace {

		std::vector<FOutlineCurvePoints> createSquareBracket(bool left)
		{
			std::vector<FOutlineCurvePoints> res;
			res.emplace_back();
			res.back().points[0] = { 0,0 };
			res.back().points[1] = { 0,0 };
			res.back().points[2] = { 5,0 };
			res.emplace_back();
			res.back().points[0] = { 5,0 };
			res.back().points[1] = { 5,0 };
			res.back().points[2] = { 5,3 };
			res.emplace_back();
			res.back().points[0] = { 5,3 };
			res.back().points[1] = { 5,3 };
			res.back().points[2] = { 2,3 };
			res.emplace_back();
			res.back().points[0] = { 2,3 };
			res.back().points[1] = { 2,3 };
			res.back().points[2] = { 2,10 };
			res.emplace_back();
			res.back().points[0] = { 2,10 };
			res.back().points[1] = { 2,10 };
			res.back().points[2] = { 0,10 };
			res.emplace_back();
			res.back().points[0] = { 0,10 };
			res.back().points[1] = { 0,10 };
			res.back().points[2] = { 0,0 };
		}
	}
	MeBracket::MeBracket(wchar_t ch)
	{
		m_ch = ch;
	}

	void MeBracket::calculate(VisualToolkit* visual_toolkit)
	{

	}

	void MeBracket::draw(VisualToolkit* visual_toolkit)
	{
	}

}

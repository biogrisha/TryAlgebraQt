#include <Me/include/MeBracket.h>

namespace TryAlgebraCore
{

	namespace {

		std::vector<FOutlineCurvePoints> createSquareBracket(
			int height,
			int dashLength = 30,
			int dashHeight = 10,
			int barWidth = 10)
		{
			std::vector<FOutlineCurvePoints> res;

			const int x0 = 0;
			const int x1 = dashLength;
			const int xb = barWidth;

			const int y0 = 0;
			const int y1 = dashHeight;
			const int y2 = height - dashHeight;
			const int y3 = height;

			auto addLine = [&](int x0, int y0, int x1, int y1)
				{
					auto& curve = res.emplace_back();

					curve.points[0] = { x0, y0 };
					curve.points[1] = { (x0 + x1) / 2,
										(y0 + y1) / 2 };
					curve.points[2] = { x1, y1 };
				};

			// Outer contour, clockwise

			// Top edge
			addLine(x0, y0, x1, y0);

			// Right edge of top dash
			addLine(x1, y0, x1, y1);

			// Bottom edge of top dash, towards vertical bar
			addLine(x1, y1, xb, y1);

			// Inner vertical edge
			addLine(xb, y1, xb, y2);

			// Top edge of bottom dash
			addLine(xb, y2, x1, y2);

			// Right edge of bottom dash
			addLine(x1, y2, x1, y3);

			// Bottom edge
			addLine(x1, y3, x0, y3);

			// Outer vertical edge
			addLine(x0, y3, x0, y0);

			return res;
		}
	}

	MeBracket::MeBracket(wchar_t ch)
	{
		m_ch = ch;
	}

	void MeBracket::calculate(VisualToolkit* visual_toolkit)
	{
		auto outline = createSquareBracket(40, 5, 2, 2);
		float maxX = 0;
		float maxY = 0;
		for (auto& curve : outline)
		{
			maxX = std::max({ maxX, curve.points[0].x, curve.points[1].x, curve.points[2].x });
			maxY = std::max({ maxY, curve.points[0].y, curve.points[1].y, curve.points[2].y });
		}
		m_size = { maxX, maxY };
		m_bearing_y = m_size.y / 2;
	}

	void MeBracket::draw(VisualToolkit* visual_toolkit)
	{
		visual_toolkit->mdocState->at(1).addCustomGlyph(createSquareBracket(40, 5, 2, 2), m_pos, m_size);
	}

}

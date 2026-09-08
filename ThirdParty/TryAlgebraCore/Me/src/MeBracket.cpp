#include <Me/include/MeBracket.h>

namespace TryAlgebraCore
{

	namespace {

		std::vector<FOutlineCurvePoints> createSquareBracket(
			int height,
			int dashLength = 30,
			int dashHeight = 10,
			int barWidth = 10,
			bool rightBracket = false)
		{
			OutlineBuilder outline;
			float barOffset = rightBracket ? dashLength - barWidth : 0;
			outline.addPoint(0, 0);
			outline.addPoint(dashLength, 0);
			outline.addPoint(dashLength, dashHeight);
			outline.addPoint(0, dashHeight);
			outline.closeCurve(0, 0);

			outline.addPoint(barOffset, 0, true);
			outline.addPoint(barWidth + barOffset, 0);
			outline.addPoint(barWidth + barOffset, height);
			outline.addPoint(barOffset, height);
			outline.closeCurve(barOffset, 0);

			outline.addPoint(0, height - dashHeight, true);
			outline.addPoint(dashLength, height - dashHeight);
			outline.addPoint(dashLength, height);
			outline.addPoint(0, height);
			outline.closeCurve(0, height - dashHeight);

			return outline.outline;
		}
	}

	MeBracket::MeBracket(wchar_t ch)
	{
		m_ch = ch;
	}

	void MeBracket::calculate(VisualToolkit* visual_toolkit)
	{
		m_points = createSquareBracket(40, 5, 2, 2, m_ch == L']');
		float maxX = 0;
		float maxY = 0;
		for (auto& curve : m_points)
		{
			maxX = std::max({ maxX, curve.points[0].x, curve.points[1].x, curve.points[2].x });
			maxY = std::max({ maxY, curve.points[0].y, curve.points[1].y, curve.points[2].y });
		}
		m_size = { maxX, maxY };
		m_bearing_y = m_size.y / 2;
	}

	void MeBracket::draw(VisualToolkit* visual_toolkit)
	{
		visual_toolkit->mdocState->at(1).addCustomGlyph(m_points, m_pos, m_size);
	}

	void MeBracket::setHeight(float height)
	{
		m_height = height;
	}

}

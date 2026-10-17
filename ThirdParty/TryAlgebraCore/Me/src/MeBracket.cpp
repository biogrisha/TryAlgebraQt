#include <Me/include/MeBracket.h>
#include <algorithm>

namespace TryAlgebraCore
{

	namespace {

		constexpr float bracketWidth = 2.f;
		constexpr float bracketDashLen = 5.f;

		void addCurve(
			std::vector<FOutlineCurvePoints>& outline,
			const glm::vec2& start,
			const glm::vec2& control,
			const glm::vec2& end)
		{
			auto& curve = outline.emplace_back();
			curve.points[0] = start;
			curve.points[1] = control;
			curve.points[2] = end;
		}

		void addLine(
			std::vector<FOutlineCurvePoints>& outline,
			const glm::vec2& start,
			const glm::vec2& end)
		{
			addCurve(outline, start, (start + end) / 2.f, end);
		}

		std::vector<FOutlineCurvePoints> createSquareBracket(glm::vec2 size, float lineWidth, bool rightBracket = false)
		{
			OutlineBuilder outline;
			lineWidth = std::max<int>(std::ceil(lineWidth), 2);

			float barOffset = rightBracket ? size.x - lineWidth : 0;
			outline.addPoint(0, 0);
			outline.addPoint(size.x, 0);
			outline.addPoint(size.x, lineWidth);
			outline.addPoint(0, lineWidth);
			outline.addPoint(0, 0);

			float y = size.y - lineWidth;
			outline.addPoint(0, y);
			outline.addPoint(size.x, y);
			outline.addPoint(size.x, size.y);
			outline.addPoint(0, size.y);
			outline.addPoint(0, y);

			outline.addPoint(barOffset, 0);
			outline.addPoint(barOffset + lineWidth, 0);
			outline.addPoint(barOffset + lineWidth, size.y);
			outline.addPoint(barOffset, size.y);
			outline.addPoint(barOffset, 0);

			return outline.outline();
		}

		std::vector<FOutlineCurvePoints> createRoundBracket(glm::vec2 size, float lineWidth, bool rightBracket = false)
		{
			OutlineBuilder outline;
			lineWidth = std::max<int>(std::ceil(lineWidth), 1);
			if (rightBracket)
			{
				outline.addPoint(0, 0);
				outline.addPoint(lineWidth, 0);
				outline.addPoint(size.x, std::ceil(size.y / 2), true);
				outline.addPoint(lineWidth, size.y);
				outline.addPoint(0, size.y);
				outline.addPoint(size.x - lineWidth, std::ceil(size.y / 2), true);
				outline.addPoint(0, 0);
			}
			else
			{
				outline.addPoint(size.x, 0);
				outline.addPoint(lineWidth, std::ceil(size.y / 2), true);
				outline.addPoint(size.x, size.y);
				outline.addPoint(size.x - lineWidth, size.y);
				outline.addPoint(0, std::ceil(size.y / 2), true);
				outline.addPoint(size.x - lineWidth, 0);
				outline.addPoint(size.x, 0);
			}

			return outline.outline();
		}

		std::vector<FOutlineCurvePoints> createAngleBracket(glm::vec2 size, float lineWidth, bool rightBracket = false)
		{
			OutlineBuilder outline;
			lineWidth = std::max<int>(std::ceil(lineWidth), 1);
			if (rightBracket)
			{
				outline.addPoint(0, 0);
				outline.addPoint(lineWidth, 0);
				outline.addPoint(size.x, size.y / 2);
				outline.addPoint(lineWidth, size.y);
				outline.addPoint(0, size.y);
				outline.addPoint(size.x - lineWidth, size.y / 2);
				outline.addPoint(0, 0);

			}
			else
			{
				outline.addPoint(size.x - 0, 0);
				outline.addPoint(size.x - (size.x - lineWidth), size.y / 2);
				outline.addPoint(size.x - 0, size.y);
				outline.addPoint(size.x - lineWidth, size.y);
				outline.addPoint(size.x - size.x, size.y / 2);
				outline.addPoint(size.x - lineWidth, 0);
				outline.addPoint(size.x - 0, 0);

			}

			return outline.outline();
		}

		std::vector<FOutlineCurvePoints> createCurlyBracket(glm::vec2 size, float lineWidth, bool rightBracket = false)
		{
			OutlineBuilder outline;
			lineWidth = std::max<int>(std::ceil(lineWidth), 1);
			float halfHeight = std::max<int>(std::ceil(size.y / 2), 1);
			float widthOneFourth = std::max<int>(std::ceil(size.x / 4), 1);
			float metric1Y = lineWidth * 3;
			float metric2Y = halfHeight - lineWidth * 5;
			float metric3Y = halfHeight;
			float metric1X = widthOneFourth * 2;
			float metric2X = size.x;
			if (rightBracket)
			{
				//right border
				outline.addPoint(0, 0);
				outline.addPoint(metric1X, 0, true);
				outline.addPoint(metric1X, metric1Y);
				outline.addPoint(metric1X, metric2Y);
				outline.addPoint(metric1X, metric3Y, true);
				outline.addPoint(metric2X, metric3Y);
				outline.addPoint(metric1X, metric3Y, true);
				outline.addPoint(metric1X, size.y - metric2Y);
				outline.addPoint(metric1X, size.y - metric1Y);
				outline.addPoint(metric1X, size.y, true);
				outline.addPoint(0, size.y);

				//left border
				outline.addPoint(0, size.y - lineWidth);
				outline.addPoint(metric1X - lineWidth, size.y - lineWidth, true);
				outline.addPoint(metric1X - lineWidth, size.y - metric1Y);
				outline.addPoint(metric1X - lineWidth, size.y - metric2Y);
				outline.addPoint(metric1X - lineWidth, metric3Y, true);
				outline.addPoint(metric2X - lineWidth, metric3Y);
				outline.addPoint(metric1X - lineWidth, metric3Y, true);
				outline.addPoint(metric1X - lineWidth, metric2Y);
				outline.addPoint(metric1X - lineWidth, metric1Y);
				outline.addPoint(metric1X - lineWidth, lineWidth, true);
				outline.addPoint(0, lineWidth);

			}
			else
			{
				//left border
				outline.addPoint(size.x - 0, lineWidth);
				outline.addPoint(size.x - (metric1X - lineWidth), lineWidth, true);
				outline.addPoint(size.x - (metric1X - lineWidth), metric1Y);
				outline.addPoint(size.x - (metric1X - lineWidth), metric2Y);
				outline.addPoint(size.x - (metric1X - lineWidth), metric3Y, true);
				outline.addPoint(size.x - (metric2X - lineWidth), metric3Y);
				outline.addPoint(size.x - (metric1X - lineWidth), metric3Y, true);
				outline.addPoint(size.x - (metric1X - lineWidth), size.y - metric2Y);
				outline.addPoint(size.x - (metric1X - lineWidth), size.y - metric1Y);
				outline.addPoint(size.x - (metric1X - lineWidth), size.y - lineWidth, true);
				outline.addPoint(size.x - 0, size.y - lineWidth);

				//right border
				outline.addPoint(size.x - 0, size.y);
				outline.addPoint(size.x - metric1X, size.y, true);
				outline.addPoint(size.x - metric1X, size.y - metric1Y);
				outline.addPoint(size.x - metric1X, size.y - metric2Y);
				outline.addPoint(size.x - metric1X, metric3Y, true);
				outline.addPoint(size.x - metric2X, metric3Y);
				outline.addPoint(size.x - metric1X, metric3Y, true);
				outline.addPoint(size.x - metric1X, metric2Y);
				outline.addPoint(size.x - metric1X, metric1Y);
				outline.addPoint(size.x - metric1X, 0, true);
				outline.addPoint(size.x - 0, 0);

			}
			return outline.outline();
		}

		std::vector<FOutlineCurvePoints> createBracket(wchar_t ch, const glm::vec2 size, float scalingFactor)
		{
			switch (ch)
			{
			case L'(':
			case L')':
				return createRoundBracket(size, scalingFactor * 2, ch == L')');
			case L'{':
			case L'}':
				return createCurlyBracket(size, scalingFactor * 2, ch == L'}');
			case L'<':
			case L'>':
				return createAngleBracket(size, scalingFactor * 2, ch == L'>');
			case L'[':
			case L']':
			default:
				return createSquareBracket(size, scalingFactor * 2, ch == L']');
			}
		}
	}

	MeBracket::MeBracket(wchar_t ch)
	{
		m_ch = ch;
	}

	void MeBracket::calculate(VisualToolkit* visual_toolkit)
	{
		//geometry calculated in setHeight
		switch (m_ch)
		{
		case L'(':
		case L')':
			m_size = { 14, 30 };
			break;
		case L'{':
		case L'}':
			m_size = { 15, 30 };
			break;
		case L'<':
		case L'>':
			m_size = { 15, 30 };
			break;
		case L'[':
		case L']':
		default:
			m_size = { 10, 30 };
			break;
		}
		m_size *= m_scaling_factor;
		m_bearing_y = m_size.y / 2;
	}

	void MeBracket::draw(VisualToolkit* visual_toolkit)
	{
		const auto gSize = glyphSize();
		const auto gPos = glyphPos();
		if (m_points.empty())
		{
			visual_toolkit->mdocState->at(1)
				.addCustomGlyph(
					createBracket(m_ch, gSize, m_scaling_factor),
					gPos, gSize);
		}
		else
		{
			visual_toolkit->mdocState->at(1).addCustomGlyph(m_points, gPos, gSize);
		}
	}

	void MeBracket::setHeight(float height)
	{
		m_size.y = height;
		m_bearing_y = m_size.y / 2;
		m_points = createBracket(m_ch, glyphSize(), m_scaling_factor);
	}

	glm::vec2 MeBracket::padding() const
	{
		return glm::ceil(glm::vec2{ 3,3 } * m_scaling_factor);
	}

	glm::vec2 MeBracket::glyphSize() const
	{
		auto size = glm::ceil((m_size - (padding() * 2.f)));
		size.x = std::max<int>(3, size.x);
		return size;
	}

	glm::vec2 MeBracket::glyphPos() const
	{
		return glm::ceil(m_pos + padding());
	}

}

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

		void reverseContour(std::vector<FOutlineCurvePoints>& outline)
		{
			std::reverse(outline.begin(), outline.end());
			for (auto& curve : outline)
			{
				std::swap(curve.points[0], curve.points[2]);
			}
		}

		void mirrorContour(std::vector<FOutlineCurvePoints>& outline, float width)
		{
			for (auto& curve : outline)
			{
				for (auto& point : curve.points)
				{
					point.x = width - point.x;
				}
			}
			// Reflection changes the winding direction. Reverse the path so that
			// closing brackets use the same signed fill as opening brackets.
			reverseContour(outline);
		}

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

		std::vector<FOutlineCurvePoints> createRoundBracket(
			float height,
			float width,
			float strokeWidth,
			bool rightBracket = false)
		{
			std::vector<FOutlineCurvePoints> outline;
			auto point = [](float x, float y) { return glm::vec2{ x, y }; };

			const float middle = height / 2.f;
			addCurve(outline, point(width, 0), point(0, 0), point(0, middle));
			addCurve(outline, point(0, middle), point(0, height), point(width, height));
			addLine(outline, point(width, height), point(width - strokeWidth, height));
			addCurve(outline,
				point(width - strokeWidth, height),
				point(strokeWidth, height),
				point(strokeWidth, middle));
			addCurve(outline,
				point(strokeWidth, middle),
				point(strokeWidth, 0),
				point(width - strokeWidth, 0));
			addLine(outline, point(width - strokeWidth, 0), point(width, 0));

			// The custom-glyph shader uses signed winding. Match the direction of
			// the existing square-bracket contours.
			reverseContour(outline);
			if (rightBracket)
			{
				mirrorContour(outline, width);
			}
			return outline;
		}

		std::vector<FOutlineCurvePoints> createCurlyBracket(
			float height,
			float width,
			float strokeWidth,
			bool rightBracket = false)
		{
			std::vector<FOutlineCurvePoints> outline;
			auto point = [](float x, float y) { return glm::vec2{ x, y }; };

			const float middle = height / 2.f;
			const float upperShoulder = height * 0.3f;
			const float lowerShoulder = height * 0.7f;
			const float centerTip = width * 0.5f;

			addCurve(outline, point(width, 0), point(0, 0), point(0, upperShoulder));
			addCurve(outline,
				point(0, upperShoulder),
				point(0, height * 0.45f),
				point(centerTip, middle));
			addCurve(outline,
				point(centerTip, middle),
				point(0, height * 0.55f),
				point(0, lowerShoulder));
			addCurve(outline, point(0, lowerShoulder), point(0, height), point(width, height));
			addLine(outline, point(width, height), point(width - strokeWidth, height));
			addCurve(outline,
				point(width - strokeWidth, height),
				point(strokeWidth, height),
				point(strokeWidth, lowerShoulder));
			addCurve(outline,
				point(strokeWidth, lowerShoulder),
				point(strokeWidth, height * 0.58f),
				point(centerTip + strokeWidth, middle));
			addCurve(outline,
				point(centerTip + strokeWidth, middle),
				point(strokeWidth, height * 0.42f),
				point(strokeWidth, upperShoulder));
			addCurve(outline,
				point(strokeWidth, upperShoulder),
				point(strokeWidth, 0),
				point(width - strokeWidth, 0));
			addLine(outline, point(width - strokeWidth, 0), point(width, 0));

			reverseContour(outline);
			if (rightBracket)
			{
				mirrorContour(outline, width);
			}
			return outline;
		}

		std::vector<FOutlineCurvePoints> createBracket(wchar_t ch, float height)
		{
			switch (ch)
			{
			case L'(':
			case L')':
				return createRoundBracket(
					height, bracketDashLen, bracketWidth, ch == L')');
			case L'{':
			case L'}':
				return createCurlyBracket(
					height, bracketDashLen, bracketWidth, ch == L'}');
			case L'[':
			case L']':
			default:
				return createSquareBracket(
					height, bracketDashLen, bracketWidth, bracketWidth, ch == L']');
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
		m_size = { bracketDashLen, 20 };
		m_size *= m_scaling_factor;
		m_bearing_y = m_size.y / 2;
	}

	void MeBracket::draw(VisualToolkit* visual_toolkit)
	{
		if (!m_points.empty())
		{
			visual_toolkit->mdocState->at(1).addCustomGlyph(m_points, m_pos, m_size);
		}
		else
		{
			visual_toolkit->mdocState->at(1)
				.addCustomGlyph(
					createBracket(m_ch, m_size.y),
					m_pos, m_size);

		}
	}

	void MeBracket::setHeight(float height)
	{
		m_points = createBracket(m_ch, height);
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

}

#include <Me/include/MeSprite.h>
#include <Me/include/MeGlobals.h>
namespace TryAlgebraCore
{
	TryAlgebraCore::MeSprite::MeSprite(const std::wstring& meta)
	{
		if (meta == MeNames::spriteCursorPlacement)
		{
			m_type = Type::CursorPlacement;
		}
		else if (meta == MeNames::spritePaste)
		{
			m_type = Type::Paste;
		}

	}
	void MeSprite::calculate(VisualToolkit* visual_toolkit)
	{
		switch (m_type)
		{
		case TryAlgebraCore::MeSprite::Type::CursorPlacement:
			m_sprite.SpriteName = "mouse-arrow.png";
			break;
		case TryAlgebraCore::MeSprite::Type::Paste:
			m_sprite.SpriteName = "paste.png";
			break;
		default:
			break;
		}
		m_sprite.Size = glm::ivec2{ 30 * m_scaling_factor,30 * m_scaling_factor };
		m_sprite.Alpha = 1;

		m_size = m_sprite.Size;
		m_bearing_y = m_size.y / 2;
	}
	void MeSprite::draw(VisualToolkit* visual_toolkit)
	{
		m_sprite.Pos = m_pos;
		visual_toolkit->mdocState->at(1).addSprite(m_sprite);
	}
}

#pragma once
#include <Me/include/MeBase.h>
#include <MathDocumentRenderingStructs.h>

namespace TryAlgebraCore
{
	class MeSprite : public MeBase
	{
		TYPED_CLASS1(MeBase);

		enum class Type
		{
			CursorPlacement,
			Paste
		};
	public:
		MeSprite(const std::wstring& meta);
		virtual void calculate(VisualToolkit* visual_toolkit) override;
		virtual void draw(VisualToolkit* visual_toolkit) override;
		virtual std::wstring getName() override;
	private:
		FSpriteInstByName m_spriteData;
		Type m_type = Type::CursorPlacement;
	};
}
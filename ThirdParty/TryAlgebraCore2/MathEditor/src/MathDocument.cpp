#include <MathEditor/include/MathDocument.h>

TryAlgebraCore2::MathDocument::MathDocument()
{
	caret_path = { 1 };
}

void TryAlgebraCore2::MathDocument::type(const std::wstring& str)
{
	text_buffer.insert(str, caret_path);
}

std::vector<FGlyphData> TryAlgebraCore2::MathDocument::getRenderingData(FFreeTypeWrap* ft)
{
	std::vector<FGlyphData> glyph_data;
	TextBufferIterator it(text_buffer);
	glm::vec2 pos = {0,0};
	it.setPos({ 1 });
	while (!it.isEnd())
	{
		auto& ch = it.next();
		FGlyphData g;
		g.GlyphId.Glyph = ch.ch;
		g.GlyphId.Height = 20;
		g.Pos = pos;
		glyph_data.push_back(g);
		auto size = ft->GetGlyphSize(g.GlyphId);
		pos.x += size.x;
	}
	return glyph_data;
}

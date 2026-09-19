#include "FormulasSystem.h"
#include "FileParser.h"

namespace TryAlgebraCore::Trs
{
	void FormulasSystem::addFile(const std::wstring& document, const std::wstring& filePath)
	{
		FileParser parser;
		parser.parse(document, filePath);
	}
}

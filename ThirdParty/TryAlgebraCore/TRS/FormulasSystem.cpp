#include "FormulasSystem.h"
#include "FileParser.h"

namespace TryAlgebraCore::Trs
{
	void FormulasSystem::addFile(const std::wstring& document)
	{
		FileParser parser;
		ParsingRules rules;
		rules.tokens = {
		L"$Parsing"
		L"$SyntacticFormulas",
		L"$Formulas",
		L"$TrsRules",
		L"$Import",
		L"$TopDownExausting",
		L"$TopDownSimple",
		L"$$",
		};
		rules.identitySections = { L"$TopDownExausting", L"$TopDownSimple" };
		rules.formulaKeyword = L"$$";
		rules.importKeyword = L"$Import";
		parser.parse(document, rules);
	}
}

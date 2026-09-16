#pragma once
#include "TermTransformer.h"
#include "PatternMatchingHelpers.h"
#include "Trs.h"

namespace TryAlgebraCore::Trs
{
	class FormulasBundle
	{
		Transformer parser;
		std::vector<IdentityIntermediate> syntacticFormulas;
		std::map<std::string, std::unique_ptr<NewTrs::Term>> storage;
		std::vector<NewTrs::Term*> formulasLhs;
		std::vector<std::unique_ptr<TermIntermediate>> formulasRhs;
		int storageSize = 0;
	};

	class FormulasSystem
	{
	public:
		void parseSyntacticFormulas(const std::wstring& document);
	private:

	};
}
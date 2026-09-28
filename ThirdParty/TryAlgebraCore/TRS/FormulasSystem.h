#pragma once
#include "TermTransformer.h"
#include "PatternMatchingHelpers.h"
#include "Trs.h"
#include "FileParser.h"
#include <unordered_map>
namespace TryAlgebraCore::Trs
{
	class FormulasBundle
	{
		Transformer transformer;
		std::map<std::string, std::unique_ptr<NewTrs::Term>> storage;
		int storageSize = 0;
		std::vector<NewTrs::Term*> formulasLhs;
		std::vector<std::unique_ptr<TermIntermediate>> formulasRhs;
		std::vector<NewTrs::Identity> identities;
	};

	class FormulasSystem
	{
	public:
		void addFile(const std::wstring& document, const std::wstring& filePath);
		void compile();
	private:
		std::unordered_map<std::wstring, TrsFile> m_trsFiles;
		std::vector<ParserFile> m_parserFiles;
		std::vector<FormulasFile> m_formulasFiles;
		std::unordered_map<std::wstring, Transformer> transformers;
	};
}
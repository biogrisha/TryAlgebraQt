#pragma once
#include "TermTransformer.h"
#include "PatternMatchingHelpers.h"
#include "Trs.h"
#include "FileParser.h"
#include <unordered_map>
namespace TryAlgebraCore::Trs
{
	struct FormulasBundle
	{
		std::wstring filePath;
		Transformer* transformer = nullptr;
		NewTrs::Trs* trs = nullptr;
		std::vector<Formula> formulas;
	};

	class FormulasSystem
	{
	public:
		void addFile(const std::wstring& document, const std::wstring& filePath);
		void compile();
	private:
		void toTerm(const std::unique_ptr<TermIntermediate>& from, NewTrs::Term*& to, NewTrs::Term* parent = nullptr);
		std::unordered_map<std::wstring, TrsFile> m_trsFiles;
		std::vector<ParserFile> m_parserFiles;
		std::vector<FormulasFile> m_formulasFiles;
		std::unordered_map<std::wstring, std::unique_ptr<Transformer>> m_transformers;
		//key trs path + transformer path
		std::unordered_map<std::wstring, std::unique_ptr<NewTrs::Trs>> m_trsMap;
		std::vector<FormulasBundle> m_bundles;
	};
}
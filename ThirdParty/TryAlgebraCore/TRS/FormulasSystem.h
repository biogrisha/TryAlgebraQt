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
		Transformer* transformer = nullptr;
		NewTrs::Trs trs;
		std::vector<FormulasFile> formulaFiles;
	};

	struct FormulaRes
	{
		std::wstring formulaName;
		//expr->match variations
		std::vector<std::vector<std::wstring>> exprs;
	};

	struct FileRes
	{
		std::wstring filePath;
		std::vector<FormulaRes> formulas;
	};

	class FormulasSystem
	{
	public:
		void addFile(const std::wstring& document, const std::wstring& filePath);
		void compile();
		std::vector<FileRes> findFormulas(const std::wstring& subjString);
	private:
		void toTerm(const std::unique_ptr<TermIntermediate>& from, NewTrs::Term*& to, NewTrs::Term* parent = nullptr);
		void toIntermediate(NewTrs::Term* term, std::unique_ptr<TermIntermediate>& intermediate);
		void substitute(std::unique_ptr<TermIntermediate>& subj, const std::unique_ptr<TermIntermediate>& var, const std::unique_ptr<TermIntermediate>& sub);
		std::unordered_map<std::wstring, TrsFile> m_trsFiles;
		std::vector<ParserFile> m_parserFiles;
		std::vector<FormulasFile> m_formulasFiles;
		std::unordered_map<std::wstring, std::unique_ptr<Transformer>> m_transformers;
		//key trs path + transformer path
		std::unordered_map<std::wstring, std::unique_ptr<FormulasBundle>> m_bundles;
	};
}
#pragma once
#include "TermElaborator.h"
#include "PatternMatchingHelpers.h"
#include "Trs.h"
#include "FileParser.h"
#include <unordered_map>
namespace TryAlgebraCore::Trs
{
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

	struct TrsRule
	{
		std::vector<std::unique_ptr<TermIntermediate>> rhs;
		std::vector<std::unique_ptr<TermIntermediate>> lhs;
	};

	struct FormulasFileData
	{
		std::wstring fileName;
		std::vector<Formula> formulas;
	};

	struct FormulasBundle
	{
		std::vector<FormulasFileData> formulaFiles;
		std::vector<TrsRule> trsRules;
		const TermElaborator* termElaborator;
	};

	class FormulasSystem
	{
	public:
		void clear();
		void addFile(const std::wstring& document, const std::wstring& filePath);
		void compile();
		void findFormulas(const std::wstring& subjString);
		const std::vector<FileRes>& lastRes() const;
		const std::unordered_map<std::wstring, std::unique_ptr<TermElaborator>>& termElaborators() const;
	private:
		void toTerm(const std::unique_ptr<TermIntermediate>& from, NewTrs::Term*& to, NewTrs::Term* parent = nullptr);
		void toIntermediate(NewTrs::Term* term, std::unique_ptr<TermIntermediate>& intermediate);
		void substitute(std::unique_ptr<TermIntermediate>& subj, const std::unique_ptr<TermIntermediate>& var, const std::unique_ptr<TermIntermediate>& sub);
		std::unique_ptr<NewTrs::Trs> createTrs(const std::wstring& trsPath, TermElaborator* transformer);

		//parsed files
		std::vector<TrsFile> m_trsFiles;
		std::vector<ParserFile> m_parserFiles;
		std::vector<FormulasFile> m_formulasFiles;


		std::unordered_map<std::wstring, std::unique_ptr<TermElaborator>> m_termElaborators;
		//key trs path + term elaborator path
		std::unordered_map<std::wstring, std::unique_ptr<FormulasBundle>> m_bundles;
		std::vector<FileRes> m_lastRes;

	};
}
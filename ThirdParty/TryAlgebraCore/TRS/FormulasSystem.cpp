#include "FormulasSystem.h"
#include "FileParser.h"

namespace TryAlgebraCore::Trs
{
	void FormulasSystem::clear()
	{
		m_trsFiles.clear();
		m_parserFiles.clear();
		m_formulasFiles.clear();
		m_termElaborators.clear();
		m_bundles.clear();
		m_lastRes.clear();
	}

	void FormulasSystem::addFile(const std::wstring& document, const std::wstring& filePath)
	{
		FileParser parser;
		auto file = parser.parse(document, filePath);
		if (auto* parserFile = std::get_if<ParserFile>(&file))
		{
			m_parserFiles.push_back(std::move(*parserFile));
		}
		else if (auto* trsFile = std::get_if<TrsFile>(&file))
		{
			m_trsFiles.push_back(std::move(*trsFile));
		}
		else if (auto* formulasFile = std::get_if<FormulasFile>(&file))
		{
			m_formulasFiles.push_back(std::move(*formulasFile));
		}
	}

	void FormulasSystem::compile()
	{
		//create term elaborators
		for (auto& parserFile : m_parserFiles)
		{
			auto termElaborator = std::make_unique<TermElaborator>(std::move(parserFile.operatorElaborator));
			m_termElaborators.emplace(parserFile.filePath, std::move(termElaborator));
		}
		m_parserFiles.clear();
	}

	void FormulasSystem::findFormulas(const std::wstring& subjString)
	{
		//m_lastRes.clear();
		//auto subjIntermediate = parseToTermIntermediate(subjString);
		//for (auto& [key, bundle] : m_bundles)
		//{
		//	auto subjCopy = copyTermIntermediate(subjIntermediate);
		//	bundle->transformer->applyAll(subjCopy);
		//	NewTrs::Term* subjTerm = nullptr;
		//	toTerm(subjCopy.back(), subjTerm);
		//	auto trs = createTrs(bundle->trsPath, bundle->transformer);
		//	trs->setSubj(subjTerm);
		//	for (auto& file : bundle->formulaFiles)
		//	{
		//		FileRes fileRes;
		//		fileRes.filePath = file.filePath;
		//		for (auto& formula : file.formulas)
		//		{
		//			FormulaRes formulaRes;
		//			formulaRes.formulaName = formula.name;
		//			for (auto& id : formula.equality)
		//			{
		//				NewTrs::Term* idTerm = nullptr;
		//				toTerm(id.back(), idTerm);
		//				auto matches = trs->run(idTerm);

		//				if (!matches.empty())
		//				{
		//					for (auto& idSub : formula.equality)
		//					{
		//						std::vector<std::wstring> variations;
		//						for (auto& match : matches)
		//						{
		//							auto idCopy = copyTermIntermediate(idSub);
		//							for (auto& [var, sub] : match)
		//							{
		//								std::unique_ptr<TermIntermediate> varInter;
		//								std::unique_ptr<TermIntermediate> subInter;
		//								toIntermediate(var, varInter);
		//								toIntermediate(sub, subInter);

		//								substitute(idCopy.back(), varInter, subInter);
		//							}
		//							variations.emplace_back();
		//							bundle->transformer->applyAllInverse(idCopy);
		//							termIntermediateToStr(idCopy, variations.back());
		//						}
		//						if (!variations.empty())
		//						{
		//							formulaRes.exprs.push_back(std::move(variations));
		//						}
		//					}
		//					if (!formulaRes.exprs.empty())
		//					{
		//						fileRes.formulas.push_back(std::move(formulaRes));
		//					}
		//					break;
		//				}

		//			}
		//		}
		//		if (!fileRes.formulas.empty())
		//		{
		//			m_lastRes.push_back(std::move(fileRes));
		//		}
		//	}
		//}
	}

	const std::vector<FileRes>& FormulasSystem::lastRes() const
	{
		return m_lastRes;
	}

	const std::unordered_map<std::wstring, std::unique_ptr<TermElaborator>>& FormulasSystem::termElaborators() const
	{
		return m_termElaborators;
	}

	void FormulasSystem::toTerm(const std::unique_ptr<TermIntermediate>& from, NewTrs::Term*& to, NewTrs::Term* parent)
	{
		to = new NewTrs::Term;
		to->isVariable = from->isVariable;
		//const auto& [it, inserted] = m_symbols.emplace(from->label, m_ch);
		//if (inserted)
		//{
		//	m_ch++;
		//	it->second = m_ch;
		//	m_symbolsInv[m_ch] = from->label;
		//}
		//to->label = std::string(1, it->second);
		to->label = std::string(from->label.begin(), from->label.end());
		to->eRep = to;
		to->eReps.push_back(to);
		if (parent)
		{
			to->parents.insert(parent);
		}
		for (auto& ch : from->children)
		{
			NewTrs::Term*& childTerm = to->children.emplace_back(nullptr);
			toTerm(ch, childTerm, to);
		}
	}

	void FormulasSystem::toIntermediate(NewTrs::Term* term, std::unique_ptr<TermIntermediate>& intermediate)
	{
		intermediate = std::make_unique<TermIntermediate>();
		//intermediate->label = m_symbolsInv[term->label.back()];
		intermediate->label = std::wstring(term->label.begin(), term->label.end());
		for (NewTrs::Term* ch : term->children)
		{
			auto& newCh = intermediate->children.emplace_back();
			toIntermediate(ch, newCh);
		}
	}

	void FormulasSystem::substitute(std::unique_ptr<TermIntermediate>& subj,
		const std::unique_ptr<TermIntermediate>& var, const std::unique_ptr<TermIntermediate>& sub)
	{
		//if (subj->isVariable)
		//{
		//	if (compare(subj.get(), var.get()))
		//	{
		//		subj = copyTermIntermediate(sub);
		//	}
		//}
		//else
		//{
		//	for (auto& ch : subj->children)
		//	{
		//		substitute(ch, var, sub);
		//	}
		//}
	}
	std::unique_ptr<NewTrs::Trs> FormulasSystem::createTrs(const std::wstring& trsPath, TermElaborator* transformer)
	{
		//auto trsFile = m_trsFiles.find(trsPath);
		//if (trsFile == m_trsFiles.end())
		//{
		//	return {};
		//}
		//std::vector<NewTrs::Identity> trsIds;

		//for (auto& trsRule : trsFile->second.rules)
		//{
		//	auto copyFrom = copyTermIntermediate(trsRule.from);
		//	transformer->applyAll(copyFrom);
		//	NewTrs::Term* termFrom = nullptr;
		//	toTerm(copyFrom.back(), termFrom);

		//	auto copyTo = copyTermIntermediate(trsRule.to);
		//	transformer->applyAll(copyTo);
		//	NewTrs::Term* termTo = nullptr;
		//	toTerm(copyTo.back(), termTo);

		//	auto& id = trsIds.emplace_back();
		//	id.lhs = termFrom;
		//	id.rhs = termTo;
		//}
		//auto trs = std::make_unique<NewTrs::Trs>();
		//trs->setIds(std::move(trsIds));
		//return trs;
		return {};
	}
}

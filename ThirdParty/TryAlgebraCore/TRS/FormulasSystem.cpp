#include "FormulasSystem.h"
#include "FileParser.h"

namespace TryAlgebraCore::Trs
{
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
			m_trsFiles.emplace(trsFile->filePath, std::move(*trsFile));
		}
		else if (auto* formulasFile = std::get_if<FormulasFile>(&file))
		{
			m_formulasFiles.push_back(std::move(*formulasFile));
		}
	}

	void FormulasSystem::compile()
	{
		for (auto& file : m_parserFiles)
		{
			std::unique_ptr<Transformer> tr = std::make_unique<Transformer>();
			tr->addRules(std::move(file.rules));
			tr->addInvRules(std::move(file.invRules));
			m_transformers.emplace(file.filePath, std::move(tr));
		}
		for (auto& [path, trsFile] : m_trsFiles)
		{
			for (auto& rule : trsFile.rules)
			{
				markVariables(rule.from);
				markVariables(rule.to);
			}
		}

		for (auto& formulasFile : m_formulasFiles)
		{
			FormulasBundle bundle;
			auto transformer = m_transformers.find(formulasFile.parserFilePath);
			if (transformer == m_transformers.end())
			{
				continue;
			}

			//trskey = trs path + transformer path
			std::wstring trsKey = formulasFile.trsFilePath;
			trsKey.insert(trsKey.end(), formulasFile.parserFilePath.begin(), formulasFile.parserFilePath.end());

			auto trsIt = m_trsMap.find(trsKey);
			if (trsIt != m_trsMap.end())
			{
				//found trs for same file and parser
				bundle.trs = trsIt->second.get();
			}
			else
			{
				//no such trs, create new
				auto trsFile = m_trsFiles.find(formulasFile.trsFilePath);
				if (trsFile == m_trsFiles.end())
				{
					continue;
				}
				std::vector<NewTrs::Identity> trsIds;

				for (auto& trsRule : trsFile->second.rules)
				{
					auto copyFrom = copyTermIntermediate(trsRule.from);
					transformer->second->applyAll(copyFrom);
					NewTrs::Term* termFrom = nullptr;
					toTerm(copyFrom.back(), termFrom);

					auto copyTo = copyTermIntermediate(trsRule.from);
					transformer->second->applyAll(copyTo);
					NewTrs::Term* termTo = nullptr;
					toTerm(copyTo.back(), termTo);

					auto& id = trsIds.emplace_back();
					id.lhs = termFrom;
					id.rhs = termTo;
				}

				auto trs = std::make_unique<NewTrs::Trs>();
				trs->setIds(std::move(trsIds));
				bundle.trs = trs.get();
				m_trsMap.emplace(trsKey, std::move(trs));
			}

			for (auto& formula : formulasFile.formulas)
			{
				for (auto& expr : formula.equality)
				{
					markVariables(expr);
					transformer->second->applyAll(expr);
				}
			}
			bundle.formulas = std::move(formulasFile.formulas);
			bundle.filePath = formulasFile.filePath;
			m_bundles.push_back(std::move(bundle));
		}
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
}

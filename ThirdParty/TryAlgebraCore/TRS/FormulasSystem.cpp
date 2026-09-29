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

		for (auto& formulasFile : m_formulasFiles)
		{
			auto transformer = m_transformers.find(formulasFile.parserFilePath);
			if (transformer == m_transformers.end())
			{
				continue;
			}
			auto trsFile = m_trsFiles.find(formulasFile.trsFilePath);
			if (trsFile == m_trsFiles.end())
			{
				continue;
			}

			std::wstring trsKey = formulasFile.trsFilePath;
			trsKey.insert(trsKey.end(), formulasFile.parserFilePath.begin(), formulasFile.parserFilePath.end())
				std::vector<NewTrs::Identity> trsIds;
			for (auto& trsRule : trsFile->second.rules)
			{
				auto copy = copyTermIntermediate(trsRule.from);
				transformer->second->applyAll(copy);
				NewTrs::Term* term = nullptr;
				toTerm(copy.back(), term);

			}
			//transformer->second->applyAll()
			//

			//FormulasBundle bundle;
			//bundle.transformer = trans->second.get();
			//trsFile.
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

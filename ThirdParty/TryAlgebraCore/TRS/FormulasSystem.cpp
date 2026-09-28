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
			Transformer tr;
			tr.addRules(std::move(file.rules));
		}

		for (auto& formulasFile : m_formulasFiles)
		{
			auto parserFile = m_parserFiles.find(formulasFile.parserFilePath);
			if (parserFile == m_parserFiles.end())
			{
				continue;
			}
			auto trsFile = m_trsFiles.find(formulasFile.trsFilePath);
			if (trsFile == m_trsFiles.end())
			{
				continue;
			}
			FormulasBundle bundle;
			bundle.
		}
	}
}

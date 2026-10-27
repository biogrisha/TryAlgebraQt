#include <Controls/MeActionsControl.h>
#include <ApplicationModel.h>
#include <AppGlobal.h>
#include <Models/DocumentsModel.h>
#include <TRS/PatternMatchingHelpers.h>
void MeActionsControl::parse(const QString& parserName)
{
	/*const auto& parsers = AppGlobal::appMod->formulasSystem()->transformers();
	auto parser = parsers.find(parserName.toStdWString());
	if (parser != parsers.end())
	{
		auto* currDoc = AppGlobal::appMod->docModel()->currDoc();
		auto selectedText = currDoc->meDoc()->getSelectedText();
		auto term = TryAlgebraCore::Trs::parseToTermIntermediate(selectedText);
		parser->second->applyAll(term, false);
		std::wstring res;
		TryAlgebraCore::Trs::termIntermediateToStr(term, res);
		currDoc->meDoc()->type(res, true);
	}*/
}

void MeActionsControl::parseInverse(const QString& parserName)
{
	/*const auto& parsers = AppGlobal::appMod->formulasSystem()->transformers();
	auto parser = parsers.find(parserName.toStdWString());
	if (parser != parsers.end())
	{
		auto* currDoc = AppGlobal::appMod->docModel()->currDoc();
		auto selectedText = currDoc->meDoc()->getSelectedText();
		auto term = TryAlgebraCore::Trs::parseToTermIntermediate(selectedText);
		parser->second->applyAllInverse(term, false);
		std::wstring res;
		TryAlgebraCore::Trs::termIntermediateToStr(term, res);
		currDoc->meDoc()->type(res, true);
	}*/
}

void MeActionsControl::applyFormula(MeActionData* actionData, int part)
{
	if (auto* formulaRes = qobject_cast<MeFormulaRes*>(actionData))
	{
		auto* currDoc = AppGlobal::appMod->docModel()->currDoc();
		currDoc->meDoc()->type(formulaRes->m_data->exprs[part - 1].back(), true);
	}
}

void MeActionsControl::scaleMatrix(MatrixScalingData* actionData, int row, int column)
{
	if (!actionData)
	{
		return;
	}

	const std::wstring scaledMatrix = actionData->m_matrixScaling.scale(row, column);
	if (scaledMatrix.empty())
	{
		return;
	}

	auto* currDoc = AppGlobal::appMod->docModel()->currDoc();
	currDoc->meDoc()->type(scaledMatrix, true);
}

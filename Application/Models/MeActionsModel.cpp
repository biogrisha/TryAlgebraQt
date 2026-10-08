#include "MeActionsModel.h"
#include <ApplicationModel.h>
#include <AppGlobal.h>

MeActionData::MeActionData(QObject* parent)
	: QObject(parent)
{

}

QString MeActionData::name() const
{
	return m_name;
}

QString MeActionData::type() const
{
	return m_type;
}

MeActionSection::MeActionSection(QObject* parent)
	:QObject(parent)
{

}

QString MeActionSection::name() const
{
	return m_name;
}

QList<MeActionData*> MeActionSection::actions() const
{
	return m_actions;
}


void MeActionsModel::update()
{

	for (MeActionSection* section : m_sections)
	{
		delete section;
	}
	m_sections.clear();

	auto addAction = [](MeActionSection* section, const QString& name, const QString& type)
		{
			auto* action = new MeActionData(section);
			action->m_name = name;
			action->m_type = type;
			section->m_actions.append(action);
		};

	const auto& parsers = AppGlobal::appMod->formulasSystem()->transformers();
	if (!parsers.empty())
	{
		auto* section = new MeActionSection(this);
		section->m_name = QStringLiteral("Parsers");
		for (const auto& parser : parsers)
		{
			addAction(section, QString::fromStdWString(parser.first), QStringLiteral("parser"));
		}
		m_sections.append(section);
	}
	foundFormulas = AppGlobal::appMod->formulasSystem()->lastRes();
	if (!foundFormulas.empty())
	{
		auto* section = new MeActionSection(this);
		section->m_name = QStringLiteral("Formulas");
		for (const auto& file : foundFormulas)
		{
			for (const auto& formula : file.formulas)
			{
				auto* action = new MeFormulaRes(section);
				action->m_name = QString::fromStdWString(formula.formulaName);
				action->m_type = "formula";
				action->m_data = &formula;
				section->m_actions.append(action);
			}
		}
		m_sections.append(section);
	}
}

QList<MeActionSection*> MeActionsModel::sections() const
{
	return m_sections;
}

MeFormulaRes::MeFormulaRes(QObject* parent)
	: MeActionData(parent)
{
}

int MeFormulaRes::partsNum() const
{
	return m_data->exprs.size();
}

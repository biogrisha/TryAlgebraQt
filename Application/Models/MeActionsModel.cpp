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
		auto* section1 = new MeActionSection(this);
		section1->m_name = QStringLiteral("Parsers");
		for (const auto& parser : parsers)
		{
			addAction(section1, QString::fromStdWString(parser.first), QStringLiteral("parser"));
		}
		m_sections.append(section1);
	}
}

QList<MeActionSection*> MeActionsModel::sections() const
{
	return m_sections;
}

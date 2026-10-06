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
	//m_data.clear();
	//m_transformers.clear();
	//const auto& trans = AppGlobal::appMod->formulasSystem()->transformers();
	//for (const auto& tr : trans)
	//{
	//	m_transformers.push_back(QString::fromStdWString(tr.first));
	//}

	//auto& transCategory = m_data.emplace_back(std::make_unique<Node>());
	//transCategory->type = Node::Type::TransCategory;
	//int row = 0;
	//for (const auto& tr : m_transformers)
	//{
	//	auto& node = transCategory->children.emplace_back(std::make_unique<Node>());
	//	node->type = Node::Type::Transformer;
	//	node->data = &tr;
	//	node->indexInParent = row;
	//	node->parent = transCategory.get();
	//	++row;
	//}

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

	auto* section1 = new MeActionSection(this);
	section1->m_name = QStringLiteral("Section 1");
	addAction(section1, QStringLiteral("Action 1"), QStringLiteral("Type 1"));
	addAction(section1, QStringLiteral("Action 2"), QStringLiteral("Type 2"));
	addAction(section1, QStringLiteral("Action 2"), QStringLiteral("Type 2"));
	addAction(section1, QStringLiteral("Action 2"), QStringLiteral("Type 2"));
	addAction(section1, QStringLiteral("Action 2"), QStringLiteral("Type 2"));
	addAction(section1, QStringLiteral("Action 2"), QStringLiteral("Type 2"));
	addAction(section1, QStringLiteral("Action 2"), QStringLiteral("Type 2"));
	addAction(section1, QStringLiteral("Action 2"), QStringLiteral("Type 2"));
	m_sections.append(section1);

	auto* section2 = new MeActionSection(this);
	section2->m_name = QStringLiteral("Section 2");
	addAction(section2, QStringLiteral("Action 3"), QStringLiteral("Type 1"));
	addAction(section2, QStringLiteral("Action 4"), QStringLiteral("Type 2"));
	m_sections.append(section2);
}

QList<MeActionSection*> MeActionsModel::sections() const
{
	return m_sections;
}

#include "MeActionsModel.h"
#include <ApplicationModel.h>
#include <AppGlobal.h>

void MeActionsModel::update()
{
	m_transformers.clear();
	const auto& trans = AppGlobal::appMod->formulasSystem()->transformers();
	for (const auto& tr : trans)
	{
		m_transformers.push_back(QString::fromStdWString(tr.first));
	}
	int row = 0;
	for (const auto& tr : m_transformers)
	{
		auto& node = m_data.emplace_back(std::make_unique<Node>());
		node->type = Node::Type::Transformer;
		node->data = &tr;
		node->indexInParent = row;
		++row;
	}
}

QModelIndex MeActionsModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!parent.isValid())
	{
		return createIndex(row, column, m_data[row].get());
	}
	return QModelIndex();
}

QModelIndex MeActionsModel::parent(const QModelIndex& child) const
{
	if (Node* data = static_cast<Node*>(child.internalPointer()))
	{
		if (Node* parent = data->parent)
		{
			return createIndex(parent->indexInParent, 0, parent);
		}
	}
}

#include "MeActionsModel.h"
#include <ApplicationModel.h>
#include <AppGlobal.h>

void MeActionsModel::update()
{
	beginResetModel();
	m_data.clear();
	m_transformers.clear();
	const auto& trans = AppGlobal::appMod->formulasSystem()->transformers();
	for (const auto& tr : trans)
	{
		m_transformers.push_back(QString::fromStdWString(tr.first));
	}

	auto& transCategory = m_data.emplace_back(std::make_unique<Node>());
	transCategory->type = Node::Type::TransCategory;
	int row = 0;
	for (const auto& tr : m_transformers)
	{
		auto& node = transCategory->children.emplace_back(std::make_unique<Node>());
		node->type = Node::Type::Transformer;
		node->data = &tr;
		node->indexInParent = row;
		node->parent = transCategory.get();
		++row;
	}
	endResetModel();
}

QModelIndex MeActionsModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!parent.isValid())
	{
		return createIndex(row, column, m_data[row].get());
	}
	auto* node = static_cast<Node*>(parent.internalPointer());
	return createIndex(row, column, node->children[row].get());
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
	return QModelIndex();
}

int MeActionsModel::rowCount(const QModelIndex& parent) const
{
	if (!parent.isValid())
	{
		return m_data.size();
	}
	auto* node = static_cast<Node*>(parent.internalPointer());
	return node->children.size();
}

int MeActionsModel::columnCount(const QModelIndex& parent) const
{
	return 1;
}

QVariant MeActionsModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (auto* node = static_cast<Node*>(index.internalPointer()))
		{
			if (node->type == Node::Type::Transformer)
			{
				auto transformer = static_cast<const QString*>(node->data);
				return *transformer;
			}
			else if (node->type == Node::Type::TransCategory)
			{
				return "Parsers";
			}
		}
	}
	return QVariant();
}

#pragma once
#include <QAbstractItemModel>
#include <TRS/FormulasSystem.h>
#include <qqml.h>
class MeActionsModel : public QAbstractItemModel
{
	Q_OBJECT
		QML_ELEMENT

		struct Node
	{
		enum class Type
		{
			File,
			Formula,
			Transformer
		};

		Type type;

		int indexInParent = 0;
		Node* parent = nullptr;
		std::vector<std::unique_ptr<Node>> children;
		const void* data = nullptr;
	};

public:
	MeActionsModel() = default;
	void update();
	QModelIndex index(int row, int column,
		const QModelIndex& parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex& child) const override;

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
private:
	QStringList m_transformers;
	std::vector<TryAlgebraCore::Trs::FileRes> m_foundFormulas;
	std::vector<std::unique_ptr<Node>> m_data;
};
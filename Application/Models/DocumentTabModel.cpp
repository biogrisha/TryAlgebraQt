#include "DocumentTabModel.h"

DocumentTabInfo::DocumentTabInfo(const QString& inFileName, const QString& inFilePath)
{
	m_fileName = inFileName;
	m_filePath = inFilePath;
}

QString DocumentTabInfo::fileName() const
{
	return m_fileName;
}

QString DocumentTabInfo::filePath() const
{
	return m_filePath;
}

DocumentTabInfoModel::DocumentTabInfoModel(QObject* parent)
	:QAbstractListModel(parent)
{
}

int DocumentTabInfoModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return m_documents.count();
}

QVariant DocumentTabInfoModel::data(const QModelIndex& index, int role) const
{
	if (index.row() < 0 || index.row() >= m_documents.count())
		return QVariant();

	const DocumentTabInfo& docInfo = m_documents[index.row()];
	if (role == DocumentTabInfoRoles::FileName)
		return docInfo.fileName();
	if (role == DocumentTabInfoRoles::FilePath)
		return docInfo.filePath();
	
	return QVariant();
}

void DocumentTabInfoModel::addDocumentTabInfo(const DocumentTabInfo& Info)
{
	beginInsertRows(QModelIndex(), rowCount(), rowCount());
	m_documents << Info;
	endInsertRows();
}

void DocumentTabInfoModel::removeDocumentTabInfo(const QString& fileName)
{
	//find index
	int i = 0;
	bool foundFile = false;
	for (; i < m_documents.size(); i++)
	{
		if (m_documents[i].fileName() == fileName)
		{
			foundFile = true;
			break;
		}
	}
	if (!foundFile)
	{
		return;
	}

	beginRemoveRows(QModelIndex(), i, i);
	m_documents.removeAt(i);
	endRemoveRows();
}

void DocumentTabInfoModel::removeDocumentTabInfo(qint32 ind)
{
	beginRemoveRows(QModelIndex(), ind, ind);
	m_documents.removeAt(ind);
	endRemoveRows();
}

QHash<int, QByteArray> DocumentTabInfoModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles[DocumentTabInfoRoles::FileName] = "fileName";
	roles[DocumentTabInfoRoles::FilePath] = "filePath";
	return roles;
}

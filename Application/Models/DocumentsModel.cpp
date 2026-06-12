#include "DocumentsModel.h"
#include <qfileinfo.h>

DocumentInfo::DocumentInfo(const QString& filePath, std::unique_ptr<TryAlgebraCore::MathDocument>&& meDoc)
	: m_filePath(filePath)
	, m_meDoc(std::move(meDoc))
{
}

QString DocumentInfo::filePath() const
{
	return m_filePath;
}

TryAlgebraCore::MathDocument* DocumentInfo::meDoc()
{
	return m_meDoc.get();
}

DocumentsModel::DocumentsModel(QObject* parent)
	:QAbstractListModel(parent)
{
}

int DocumentsModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return m_documents.size();
}

QVariant DocumentsModel::data(const QModelIndex& index, int role) const
{
	if (index.row() < 0 || index.row() >= m_documents.size())
		return QVariant();

	const DocumentInfo& docInfo = m_documents[index.row()];
	if (role == DocumentTabInfoRoles::FilePath)
		return docInfo.filePath();
	if (role == DocumentTabInfoRoles::FileName)
	{
		QFileInfo fileInfo(docInfo.filePath());
		return fileInfo.fileName();
	}
	
	return QVariant();
}

DocumentInfo* DocumentsModel::docInfo(const QModelIndex& index)
{
	if (index.row() < 0 || index.row() >= m_documents.size())
		return nullptr;
	return &m_documents[index.row()];
}

DocumentInfo* DocumentsModel::docInfo(const QString& filePath)
{
	for (auto& doc : m_documents)
	{
		if (doc.filePath() == filePath)
		{
			return &doc;
		}
	}
	return nullptr;
}

void DocumentsModel::addDocInfo(DocumentInfo&& Info)
{
	beginInsertRows(QModelIndex(), rowCount(), rowCount());
	m_documents.push_back(std::move(Info));
	endInsertRows();
	onDocumentAdded(&m_documents.back());
}

void DocumentsModel::removeDocInfo(const QString& fileName)
{
	//find index
	int i = 0;
	bool foundFile = false;
	for (; i < m_documents.size(); i++)
	{
		if (m_documents[i].filePath() == fileName)
		{
			foundFile = true;
			break;
		}
	}
	if (!foundFile)
	{
		return;
	}
	onBeforeDocRemoved(&m_documents[i]);
	beginRemoveRows(QModelIndex(), i, i);
	m_documents.erase(m_documents.begin() + i);
	endRemoveRows();
}

void DocumentsModel::removeDocInfo(qint32 ind)
{
	onBeforeDocRemoved(&m_documents[ind]);
	beginRemoveRows(QModelIndex(), ind, ind);
	m_documents.erase(m_documents.begin() + ind);
	endRemoveRows();
}

void DocumentsModel::setCurrentDocument(const QString& fileName)
{
	m_curDocPath = fileName;
	onCurrentDocChanged(m_curDocPath.value());
}

std::optional<QString> DocumentsModel::curDocPath()
{
	return m_curDocPath;
}

bool DocumentsModel::isDocumentOpened(const QString& filePath)
{
	for (auto& doc : m_documents)
	{
		if (doc.filePath() == filePath)
		{
			return true;
		}
	}
	return false;
}

QHash<int, QByteArray> DocumentsModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles[DocumentTabInfoRoles::FilePath] = "filePath";
	roles[DocumentTabInfoRoles::FileName] = "fileName";
	return roles;
}

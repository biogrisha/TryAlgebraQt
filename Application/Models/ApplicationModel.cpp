#include "ApplicationModel.h"

ApplicationModel::ApplicationModel(QObject* parent)
	: QObject(parent)
	, m_meListModel(new MeListModel(this))
	, m_docModel(new DocumentsModel(this))
	, m_fileSystemModel(new QFileSystemModel(this))
{
	m_fileSystemModel->setRootPath("C:\\");
}

DocumentsModel* ApplicationModel::docModel()
{
	return m_docModel;
}

MeListModel* ApplicationModel::meListModel()
{
	return m_meListModel;
}

TryAlgebraCore::KeyBinding* ApplicationModel::keyBinding()
{
	return &m_keyBinding;
}

QFileSystemModel* ApplicationModel::fileSystemModel()
{
	return m_fileSystemModel;
}

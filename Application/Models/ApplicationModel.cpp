#include "ApplicationModel.h"
#include <QSettings>

namespace SettingKeys {
	constexpr const char* RecentFolders = "recentFolders";
}

ApplicationModel::ApplicationModel(QObject* parent)
	: QObject(parent)
	, m_meListModel(new MeListModel(this))
	, m_docModel(new DocumentsModel(this))
	, m_fileSystemModel(new QFileSystemModel(this))
{

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

QStringList ApplicationModel::recentFolders() const
{
	QSettings settings;
	QStringList recentFolders = settings.value(SettingKeys::RecentFolders).toStringList();
	std::reverse(recentFolders.begin(), recentFolders.end());
	return recentFolders;
}

QModelIndex ApplicationModel::fileSystemRootIndex() const
{
	return m_fileSystemRootIndex;
}

QString ApplicationModel::projectFolder() const
{
	return m_projectFolder;
}

void ApplicationModel::setCurrentFolder(QString currentFolder)
{
	m_projectFolder = std::move(currentFolder);
	m_fileSystemRootIndex = m_fileSystemModel->setRootPath(m_projectFolder);
	QSettings settings;
	QStringList recentFolders = settings.value(SettingKeys::RecentFolders).toStringList();
	recentFolders.removeAll(m_projectFolder);
	recentFolders.append(m_projectFolder);
	settings.setValue(SettingKeys::RecentFolders, recentFolders);
}

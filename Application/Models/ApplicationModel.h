#pragma once
#include <QObject>
#include <Models/MeListModel.h>
#include <Models/DocumentsModel.h>
#include <qqml.h>
#include <MathEditor/include/KeyBinding.h>
#include <QFileSystemModel>

class ApplicationModel : public QObject
{
	Q_OBJECT
		QML_ELEMENT
public:
	ApplicationModel(QObject* parent = nullptr);
	void setCurrentFolder(QString currentFolder);

signals:
	void onNewDoc();

public slots:
	MeListModel* meListModel();
	DocumentsModel* docModel();
	TryAlgebraCore::KeyBinding* keyBinding();
	QFileSystemModel* fileSystemModel();
	QStringList recentFolders() const;
	QModelIndex fileSystemRootIndex() const;
	QString projectFolder() const;
private:
	MeListModel* m_meListModel;
	DocumentsModel* m_docModel;
	TryAlgebraCore::KeyBinding m_keyBinding;
	QFileSystemModel* m_fileSystemModel;
	QString m_projectFolder;
	QModelIndex m_fileSystemRootIndex;
};
#pragma once
#include <qqml.h>
#include <QObject>
#include <Models/MeListModel.h>
#include <Models/DocumentsModel.h>
#include <MathEditor/include/KeyBinding.h>
#include <QFileSystemModel>

class ApplicationModel : public QObject
{
	Q_OBJECT
		QML_ELEMENT
public:
	ApplicationModel(QObject* parent = nullptr);
signals:
	void onNewDoc();

public slots:
	MeListModel* meListModel();
	DocumentsModel* docModel();
	TryAlgebraCore::KeyBinding* keyBinding();
	QFileSystemModel* fileSystemModel();
private:
	MeListModel* m_meListModel;
	DocumentsModel* m_docModel;
	TryAlgebraCore::KeyBinding m_keyBinding;
	QFileSystemModel* m_fileSystemModel;
};
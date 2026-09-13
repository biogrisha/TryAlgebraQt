#pragma once
#include <QObject>
#include <QString>
#include <QUrl>
#include <qqml.h>
class FileSystemViewControl : public QObject
{
	Q_OBJECT
		QML_ELEMENT
public:
	FileSystemViewControl() = default;
public slots:
	void selectFile(const QModelIndex& index);
};
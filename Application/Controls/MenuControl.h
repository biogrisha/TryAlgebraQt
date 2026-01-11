#pragma once
#include <QObject>
#include <QString>
#include <QUrl>
#include <qqml.h>
/*
* Control used to handle menu events
*/
class MenuControl : public QObject
{
	Q_OBJECT
	QML_ELEMENT
public:
	MenuControl(QObject* parent);
	MenuControl() = default;
public slots:
	//Opens document by url
	void openDocument(const QUrl& url);
};
#pragma once

#include <QObject>
#include <QString>
#include <QUrl>
#include <qqml.h>

class MenuControl : public QObject
{
	Q_OBJECT
	QML_ELEMENT
public:
	MenuControl(QObject* parent);
	MenuControl() = default;
public slots:
	void openDocument(const QUrl& url);
};
#pragma once
#include <QObject>
#include <QString>
#include <QUrl>
#include <qqml.h>
#include <Models/DocumentsModel.h>

/*
* Control used to handle interaction with tabs
*/
class TabsControl : public QObject
{
	Q_OBJECT
	QML_ELEMENT
public:
	TabsControl() = default;
	TabsControl(QObject* parent);
public slots:
	void selectTab(qint32 id);

	void closeTab(qint32 id);

	void setCurrentTabId(qint32 id);
};
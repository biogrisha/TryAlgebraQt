#pragma once
#include <QObject>
#include <QString>
#include <QUrl>
#include <qqml.h>
#include <FunctionLibraries/FileHelpers.h>
#include <Modules/CommonTypes/MulticastDelegate.h>
#include <Models/DocumentTabModel.h>

/*
* Control used to handle interaction with tabs
*/
class TabsControl : public QObject
{
	Q_OBJECT
	QML_ELEMENT
public:
	Q_PROPERTY(qint32 currentTabId READ currentTabId WRITE setCurrentTabId NOTIFY currentTabIdChanged)
	TabsControl() = default;
	TabsControl(QObject* parent);
public slots:
	//Selects new doc by tab id
	void selectTab(qint32 id);

	//Closes doc by tab id
	void closeTab(qint32 id);

	//Returns model containin tabs info. Note that the model is owned by TabsControl
	DocumentTabInfoModel* getTabsModel();
	
	//---properties

	qint32 currentTabId();
	void setCurrentTabId(qint32 id);


	//---handlers
	
	void onCurrentDocumentChanged(qint32 ind);
	void onDocumentOpened(qint32 ind);
	void onDocumentClosed(qint32 ind);

signals:
	void currentTabIdChanged(qint32 id);
private:
	//Model containing information of displayed tabs
	DocumentTabInfoModel* m_tabInfoModel = nullptr;
	
	//Currently selected tab(equals to current doc ind)
	qint32 m_currentTabId = 0;
};
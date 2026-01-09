#pragma once
#include <QObject>
#include <QString>
#include <QUrl>
#include <qqml.h>
#include "FunctionLibraries/FileHelpers.h"
#include "Modules/CommonTypes/MulticastDelegate.h"
#include "Models/DocumentTabModel.h"

class TabsControl : public QObject
{
	Q_OBJECT
	QML_ELEMENT
public:
	Q_PROPERTY(qint32 currentTabId READ currentTabId WRITE setCurrentTabId NOTIFY currentTabIdChanged)
	TabsControl() = default;
	TabsControl(QObject* parent);
public slots:
	void selectTab(qint32 id);
	void closeTab(qint32 id);
	DocumentTabInfoModel* getTabsModel();

	qint32 currentTabId();
	void setCurrentTabId(qint32 id);

	void onCurrentDocumentChanged(qint32 ind);
	void onDocumentOpened(qint32 ind);
signals:
	void currentTabIdChanged(qint32 id);
private:
	void onDocumentAdded(const std::weak_ptr<FTAMathDocumentInfo>& docInfo);
	FTAMulticastDelegate<const std::weak_ptr<FTAMathDocumentInfo>&>::HndlPtr m_onAddedHndl;
	DocumentTabInfoModel* m_tabInfoModel = nullptr;
	qint32 m_currentTabId = 0;
};
#include "TabsControl.h"
#include <QDebug>
#include "Modules/MainWindowModule.h"
#include "FilesControl.h"
#include "Application.h"
#include "AppGlobal.h"

TabsControl::TabsControl(QObject* parent)
	:QObject(parent)
{
	m_tabInfoModel = new DocumentTabInfoModel(this);
	auto filesControl = AppGlobal::application->getFilesControl();
	QObject::connect(filesControl, &FilesControl::onDocumentOpened, this, &TabsControl::onDocumentOpened);
	QObject::connect(filesControl, &FilesControl::onCurrentDocumentChanged, this, &TabsControl::onCurrentDocumentChanged);
	QObject::connect(filesControl, &FilesControl::onDocumentClosed, this, &TabsControl::onDocumentClosed);
}

void TabsControl::selectTab(qint32 id)
{
	if (id == m_currentTabId)
	{
		return;
	}
	AppGlobal::application->getFilesControl()->selectDocument(id);
}

void TabsControl::closeTab(qint32 id)
{
	AppGlobal::application->getFilesControl()->closeDocument(id);
}

DocumentTabInfoModel* TabsControl::getTabsModel()
{
	return m_tabInfoModel;
}

qint32 TabsControl::currentTabId()
{
	return m_currentTabId;
}

void TabsControl::setCurrentTabId(qint32 id)
{
	if (m_currentTabId == id)
	{
		return;
	}
	m_currentTabId = id;
	emit currentTabIdChanged(id);
}

void TabsControl::onCurrentDocumentChanged(qint32 ind)
{
	setCurrentTabId(ind);
}

void TabsControl::onDocumentOpened(qint32 ind)
{
	auto& docInfo = AppGlobal::mainModule->GetAllDocuments()[ind];
	m_tabInfoModel->addDocumentTabInfo(DocumentTabInfo(QString::fromStdWString(docInfo->FileName), QString::fromStdWString(docInfo->FilePath)));
}

void TabsControl::onDocumentClosed(qint32 ind)
{
	m_tabInfoModel->removeDocumentTabInfo(ind);
}


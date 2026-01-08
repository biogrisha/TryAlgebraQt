#include "TabsControl.h"
#include <QDebug>
#include "Modules/MainWindowModule.h"
#include "AppGlobal.h"

TabsControl::TabsControl(QObject* parent)
	:QObject(parent)
{
	m_onAddedHndl = AppGlobal::mainModule->OnDocumentAdded.AddFunc(this, &TabsControl::onDocumentAdded);
	m_tabInfoModel = new DocumentTabInfoModel(this);
}

void TabsControl::selectTab(qint32 id)
{
	if (id == m_currentTabId)
	{
		return;
	}
	setCurrentTabId(id);
	auto docs = AppGlobal::mainModule->GetAllDocuments();

}

void TabsControl::closeTab(qint32 id)
{
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

void TabsControl::onDocumentAdded(const std::weak_ptr<FTAMathDocumentInfo>& docInfo)
{
	auto docInfoPtr = docInfo.lock();
	m_tabInfoModel->addDocumentTabInfo(DocumentTabInfo(QString::fromStdWString(docInfoPtr->FileName), QString::fromStdWString(docInfoPtr->FilePath)));
	setCurrentTabId(m_tabInfoModel->rowCount() - 1);
}


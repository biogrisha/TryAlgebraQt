#include "TabsControl.h"
//#include "Modules/MainWindowModule.h"
#include "AppGlobal.h"

TabsControl::TabsControl(QObject* parent)
	:QObject(parent)
{
	//m_onAddedHndl = AppGlobal::mainModule->OnDocumentAdded.AddFunc(this, &TabsControl::onDocumentAdded);
	m_tabInfoModel = new DocumentTabInfoModel(this);
}

void TabsControl::selectTab(const QString& fileName)
{
}

void TabsControl::closeTab(const QString& fileName)
{
}

void TabsControl::onDocumentAdded(const std::weak_ptr<FTAMathDocumentInfo>& docInfo)
{
	//m_tabInfoModel->addDocumentTabInfo()
}


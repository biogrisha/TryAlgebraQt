#include "TabsControl.h"
#include <QDebug>
#include <Application.h>
#include <AppGlobal.h>
#include <Controls/FilesControl.h>
#include <ApplicationModel.h>
#include <DocumentsModel.h>

TabsControl::TabsControl(QObject* parent)
	:QObject(parent)
{
	
}

void TabsControl::selectTab(qint32 id)
{
	
}

void TabsControl::closeTab(qint32 id)
{
	DocumentsModel* docModel = AppGlobal::appMod->docModel();
	docModel->removeDocInfo(id);
}

void TabsControl::setCurrentTabId(qint32 id)
{

}



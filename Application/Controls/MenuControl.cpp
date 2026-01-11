#include "Controls/MenuControl.h"
#include <Application.h>
#include <AppGlobal.h>
#include <Controls/FilesControl.h>

MenuControl::MenuControl(QObject* parent)
	: QObject(parent)
{
}

void MenuControl::openDocument(const QUrl& url)
{	
	//Delegate document opening to FilesControl
	auto filesControl = AppGlobal::application->getFilesControl();
	filesControl->openDocument(url);
}


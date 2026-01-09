#include "MenuControl.h"
#include "Application.h"
#include "AppGlobal.h"
#include "FilesControl.h"

MenuControl::MenuControl(QObject* parent)
	: QObject(parent)
{
}

void MenuControl::openDocument(const QUrl& url)
{	
	auto filesControl = AppGlobal::application->getFilesControl();
	filesControl->openDocument(url);
}


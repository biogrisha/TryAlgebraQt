#include "MenuControl.h"
#include "Application.h"
#include "AppGlobal.h"
#include "DocumentControl.h"

MenuControl::MenuControl(QObject* parent)
	: QObject(parent)
{
}

void MenuControl::openDocument(const QUrl& url)
{	
	auto documentControl = new DocumentControl(AppGlobal::application);
	documentControl->setDocument(url);
	AppGlobal::application->setCurrentDocument(documentControl);
}


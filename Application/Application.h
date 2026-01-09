#pragma once

#include <QObject>
#include <QString>
#include "Modules/MainWindowModule.h"
#include "FreeTypeWrap.h"

class MenuControl;
class DocumentControl;
class TabsControl;
class FilesControl;
class FFreeTypeWrap;

class Application  : public QObject
{
	Q_OBJECT
	
public:
	~Application();
	Application(QObject *parent);
	FFreeTypeWrap* getFreeTypeWrap();
	FilesControl* getFilesControl();
public slots:
	TabsControl* getTabs();
	MenuControl* getMenu();
	DocumentControl* getDocumentControl();
private:
	MenuControl* m_menuControl = nullptr;
	DocumentControl* m_documentControl = nullptr;
	TabsControl* m_tabsControl = nullptr;
	FilesControl* m_filesControl = nullptr;

	FTAMainModule m_mainModule;
	FFreeTypeWrap m_freeTypeWrap;
};

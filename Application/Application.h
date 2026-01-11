#pragma once

#include <QObject>
#include <QString>
#include <Modules/MainWindowModule.h>
#include <FreeTypeWrap.h>

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

	//Returns freetype module
	FFreeTypeWrap* getFreeTypeWrap();

	//Returns control to manipulate files
	FilesControl* getFilesControl();

public slots:
	//Returns tabs control
	TabsControl* getTabs();

	//Returns menu control
	MenuControl* getMenu();

	//Returns document control
	DocumentControl* getDocumentControl();
private:
	MenuControl* m_menuControl = nullptr;
	DocumentControl* m_documentControl = nullptr;
	TabsControl* m_tabsControl = nullptr;
	FilesControl* m_filesControl = nullptr;

	//try algebra core module
	FTAMainModule m_mainModule;
	//free type module
	FFreeTypeWrap m_freeTypeWrap;
};

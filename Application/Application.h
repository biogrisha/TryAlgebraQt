#pragma once

#include <QObject>
#include <QString>
#include "Modules/MainWindowModule.h"
#include "FreeTypeWrap.h"

class MenuControl;
class DocumentControl;
class TabsControl;
class FFreeTypeWrap;

class Application  : public QObject
{
	Q_OBJECT
	
public:
	~Application();
	Application(QObject *parent);
	void setCurrentDocument(DocumentControl* documentControl);
	FFreeTypeWrap* getFreeTypeWrap();
public slots:
	TabsControl* getTabs();
	MenuControl* getMenu();
	DocumentControl* getCurrentDocument();
private:
	MenuControl* m_menuControl = nullptr;
	DocumentControl* m_documentControl = nullptr;
	TabsControl* m_tabsControl = nullptr;

	FTAMainModule m_mainModule;
	FFreeTypeWrap m_freeTypeWrap;
};

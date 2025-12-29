#pragma once

#include <memory>
#include <QObject>
#include <QString>
#include "Controls/MenuControl.h"
//#include "Controls/DocumentControl.h"
#include "MathDocument.h"
class FTAMainModule;
class FFreeTypeWrap;

class Application  : public QObject
{
	Q_OBJECT
	
public:
	~Application();
	Application(QObject *parent);
	//void setCurrentDocument(DocumentControl* documentControl);
	FFreeTypeWrap* getFreeTypeWrap();
public slots:
	MenuControl* getMenu();
	//DocumentControl* getCurrentDocument();
private:
	MenuControl* m_menuControl = nullptr;
	//DocumentControl* m_documentControl = nullptr;
	FTAMainModule* m_mainModule = nullptr;
	FFreeTypeWrap* m_freeTypeWrap = nullptr;
};

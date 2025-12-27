#pragma once

#include <QObject>
#include <QString>
#include "Controls/MenuControl.h"
#include <memory>
class FTAMainModule;

class Application  : public QObject
{
	Q_OBJECT
	
public:
	~Application();
	Application(QObject *parent);
public slots:
	MenuControl* GetMenu();
	QString GetStr();
private:
	MenuControl* m_menuControl = nullptr;

	FTAMainModule* m_mainModule = nullptr;
};

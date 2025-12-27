#include "Application.h"
#include <Modules/MainWindowModule.h>
#include "AppGlobal.h"

Application::~Application()
{
	AppGlobal::mainModule = nullptr;
	delete m_mainModule;
}

Application::Application(QObject *parent)
	: QObject(parent)
{
	m_mainModule = new FTAMainModule;
	AppGlobal::mainModule = m_mainModule;
	m_menuControl = new MenuControl(this);
}

QString Application::GetStr()
{
	return "Application works";
}

MenuControl* Application::GetMenu()
{
	return m_menuControl;
}


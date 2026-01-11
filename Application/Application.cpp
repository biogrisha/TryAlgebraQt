#include "Application.h"
#include <QQuickWindow>
#include <QVulkanInstance>

#include <VulkanContext.h>
#include <FreeTypeWrap.h>
#include <Modules/MathElementsV2/CompatibilityData.h>

#include <MathEditor/CursorComponentGenerator.h>
#include <MathEditor/MathElementGenerator.h>
#include <Controls/MenuControl.h>
#include <Controls/DocumentControl.h>
#include <Controls/TabsControl.h>
#include <Controls/FilesControl.h>
#include <AppGlobal.h>

Application::~Application()
{
	AppGlobal::mainModule = nullptr;
	AppGlobal::application = nullptr;
}

Application::Application(QObject *parent)
	: QObject(parent)
{
	//initializing application components
	// 
	//caching global variables
	AppGlobal::application = this;
	AppGlobal::mainModule = &m_mainModule;

	//creating controls
	m_filesControl = new FilesControl(this);
	m_menuControl = new MenuControl(this);
	m_tabsControl = new TabsControl(this);
	m_documentControl = new DocumentControl(this);

	//caching dpi
	QScreen* screen = QGuiApplication::primaryScreen(); 
	qreal logicalDpiX = screen->logicalDotsPerInchX(); 
	qreal logicalDpiY = screen->logicalDotsPerInchY();

	//initializing freetype
	m_freeTypeWrap.Init(logicalDpiX, logicalDpiY);
}

DocumentControl* Application::getDocumentControl()
{
	return m_documentControl;
}

FFreeTypeWrap* Application::getFreeTypeWrap()
{
	return &m_freeTypeWrap;
}

FilesControl* Application::getFilesControl()
{
	return m_filesControl;
}

TabsControl* Application::getTabs()
{
	return m_tabsControl;
}

MenuControl* Application::getMenu()
{
	return m_menuControl;
}


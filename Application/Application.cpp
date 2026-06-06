#include "Application.h"
#include <QQuickWindow>
#include <QVulkanInstance>

#include <VulkanContext.h>
#include <FreeTypeWrap.h>

#include <Controls/MenuControl.h>
#include <Controls/DocumentControl.h>
#include <Controls/TabsControl.h>
#include <Controls/FilesControl.h>
#include <Models/ApplicationModel.h>
#include <AppGlobal.h>
#include <Modules/MeAtlasGenerator.h>

Application::~Application()
{
	AppGlobal::application = nullptr;
	AppGlobal::app_mod = nullptr;
}

Application::Application(QObject *parent)
	: QObject(parent)
{
	//initializing application components
	// 
	//caching global variables
	AppGlobal::application = this;


	//creating model
	m_app_model = new ApplicationModel(this);
	AppGlobal::app_mod = m_app_model;

	//creating controls
	m_files_control = new FilesControl(this);
	m_menu_control = new MenuControl(this);
	m_tabs_control = new TabsControl(this);
	m_document_control = new DocumentControl(this);
	//caching dpi
	QScreen* screen = QGuiApplication::primaryScreen(); 
	qreal logicalDpiX = screen->logicalDotsPerInchX(); 
	qreal logicalDpiY = screen->logicalDotsPerInchY();

	//initializing freetype
	m_ft_wrap.Init(logicalDpiX, logicalDpiY);

	prepareMeAtlas();
}

DocumentControl* Application::getDocumentControl()
{
	return m_document_control;
}

void Application::prepareMeAtlas()
{
	MeAtlasGenerator atlas_gen;
	atlas_gen.gen();
}

FFreeTypeWrap* Application::getFreeTypeWrap()
{
	return &m_ft_wrap;
}

FilesControl* Application::getFilesControl()
{
	return m_files_control;
}

TabsControl* Application::getTabs()
{
	return m_tabs_control;
}

MenuControl* Application::getMenu()
{
	return m_menu_control;
}


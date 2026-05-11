#include "Application.h"
#include <QQuickWindow>
#include <QVulkanInstance>

#include <VulkanContext.h>
#include <FreeTypeWrap.h>
#include <Modules/MathElementsV2/CompatibilityData.h>
#include <Modules/Visual/MathElementsAtlas.h>

#include <MathEditor/CursorComponentGenerator.h>
#include <MathEditor/MathElementGenerator.h>
#include <Controls/MenuControl.h>
#include <Controls/DocumentControl.h>
#include <Controls/TabsControl.h>
#include <Controls/FilesControl.h>
#include <Models/ApplicationModel.h>
#include <AppGlobal.h>

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
	PrepareMeAtlas();
	//caching dpi
	QScreen* screen = QGuiApplication::primaryScreen(); 
	qreal logicalDpiX = screen->logicalDotsPerInchX(); 
	qreal logicalDpiY = screen->logicalDotsPerInchY();

	//initializing freetype
	m_ft_wrap.Init(logicalDpiX, logicalDpiY);
}

DocumentControl* Application::getDocumentControl()
{
	return m_document_control;
}

void Application::PrepareMeAtlas()
{
	//FTAMathElementsAtlas meAtlas;
	//FMathDocumentState meDocState;
	//auto meGen = std::make_unique<MathElementGeneratorQt>();
	//meGen->m_meDocState = &meDocState;
	//meGen->initMeGenerators();
	//meAtlas.Init(meGen.get());
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


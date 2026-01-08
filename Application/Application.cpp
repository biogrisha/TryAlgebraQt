#include "Application.h"
#include <QQuickWindow>
#include <QVulkanInstance>
#include "AppGlobal.h"
#include "Modules/MathElementsV2/CompatibilityData.h"
#include "MathEditor/CursorComponentGenerator.h"
#include "MathEditor/MathElementGenerator.h"
#include "FreeTypeWrap.h"
#include "VulkanContext.h"
#include "Controls/MenuControl.h"
#include "Controls/DocumentControl.h"
#include "Controls/TabsControl.h"

Application::~Application()
{
	AppGlobal::mainModule = nullptr;
	AppGlobal::application = nullptr;
}

Application::Application(QObject *parent)
	: QObject(parent)
{
	AppGlobal::application = this;
	AppGlobal::mainModule = &m_mainModule;
	m_menuControl = new MenuControl(this);
	m_tabsControl = new TabsControl(this);

	QScreen* screen = QGuiApplication::primaryScreen(); 
	qreal logicalDpiX = screen->logicalDotsPerInchX(); 
	qreal logicalDpiY = screen->logicalDotsPerInchY();
	m_freeTypeWrap.Init(logicalDpiX, logicalDpiY);
}

DocumentControl* Application::getCurrentDocument()
{
	return m_documentControl;
}

FFreeTypeWrap* Application::getFreeTypeWrap()
{
	return &m_freeTypeWrap;
}
void Application::setCurrentDocument(DocumentControl* documentControl)
{
	if(m_documentControl)
	{
		m_documentControl->deleteLater();
	}
	m_documentControl = documentControl;
}

TabsControl* Application::getTabs()
{
	return m_tabsControl;
}

MenuControl* Application::getMenu()
{
	return m_menuControl;
}


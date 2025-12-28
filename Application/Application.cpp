#include "Application.h"
#include <QQuickWindow>
#include <QVulkanInstance>
#include <Modules/MainWindowModule.h>
#include "AppGlobal.h"
#include "Modules/MathElementsV2/CompatibilityData.h"
#include "MathEditor/CursorComponentGenerator.h"
#include "MathEditor/MathElementGenerator.h"
#include "FreeTypeWrap.h"
#include "VulkanContext.h"

Application::~Application()
{
	AppGlobal::mainModule = nullptr;
	AppGlobal::application = nullptr;
	delete m_mainModule;
	delete m_freeTypeWrap;
}

Application::Application(QObject *parent)
	: QObject(parent)
{
	AppGlobal::application = this;
	m_mainModule = new FTAMainModule;
	AppGlobal::mainModule = m_mainModule;
	m_menuControl = new MenuControl(this);
	m_freeTypeWrap = new FFreeTypeWrap();

	QScreen* screen = QGuiApplication::primaryScreen(); 
	qreal logicalDpiX = screen->logicalDotsPerInchX(); 
	qreal logicalDpiY = screen->logicalDotsPerInchY();
	m_freeTypeWrap->Init(logicalDpiX, logicalDpiY);

	m_compatibilityData = FTACompatibilityData::MakeTypedShared();
	m_compatibilityData->CursorComponentGenerator = std::make_shared<CursorComponentGeneratorQt>();
	m_compatibilityData->MeGenerator = std::make_shared<MathElementGeneratorQt>();

}

void Application::setMathDocument(QQuickItem* doc)
{
	if (auto mathDoc = qobject_cast<MathDocument*>(doc))
	{
		mathDoc->setText("hello");
	}
}
FFreeTypeWrap* Application::getFreeTypeWrap()
{
	return m_freeTypeWrap;
}
MenuControl* Application::GetMenu()
{
	return m_menuControl;
}


#include "Application.h"
#include "Application.h"
#include "Application.h"
#include <qqmlengine.h>
#include <QQuickWindow>
#include <QVulkanInstance>
#include <QQuickView>
#include <VulkanContext.h>
#include <FreeTypeWrap.h>
#include <QQuickGraphicsConfiguration>
#include <Controls/MenuControl.h>
#include <Controls/DocumentControl.h>
#include <Controls/TabsControl.h>
#include <Controls/FilesControl.h>
#include <Models/ApplicationModel.h>
#include <AppGlobal.h>
#include <Modules/MeInfoGenerator.h>
#include <Modules/ImageProvider.h>
#include <Actions/Actions.h>

Application::~Application()
{
	AppGlobal::application = nullptr;
	AppGlobal::appMod = nullptr;
}

Application::Application(QObject* parent)
	: QObject(parent)
{
	//initializing application components
	// 
	//caching global variables
	AppGlobal::application = this;


	//creating model
	m_app_model = new ApplicationModel(this);
	AppGlobal::appMod = m_app_model;

	//creating controls
	m_files_control = new FilesControl(this);
	m_menu_control = new MenuControl(this);
	//caching dpi
	QScreen* screen = QGuiApplication::primaryScreen();
	qreal logicalDpiX = screen->logicalDotsPerInchX();
	qreal logicalDpiY = screen->logicalDotsPerInchY();

	//initializing freetype
	m_ft_wrap.Init(logicalDpiX, logicalDpiY);
}

ApplicationModel* Application::applicationModel()
{
	return m_app_model;
}

void Application::projectSelectedByUrl(const QUrl& url)
{
	projectSelected(url.toLocalFile());
}

void Application::projectSelected(QString folderPath)
{
	applicationModel()->setCurrentFolder(std::move(folderPath));
	m_vulkanInst = std::make_unique<QVulkanInstance>();
	m_vulkanInst->setApiVersion(QVersionNumber(1, 4));
	//inst.setLayers({
	//"VK_LAYER_KHRONOS_validation"
	//	});
	bool ok = m_vulkanInst->create();
	if (!ok) {
		return;
	}

	QQuickGraphicsConfiguration config;
	config.setDeviceExtensions({
	VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	VK_KHR_SPIRV_1_4_EXTENSION_NAME,
	VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
	VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
	VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
	VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
	VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
	QByteArrayLiteral("VK_KHR_get_memory_requirements2")
		});

	m_mainWindow = std::make_unique<QQuickView>(m_qmlEngine, nullptr);
	m_mainWindow->setGraphicsConfiguration(config);
	m_mainWindow->setResizeMode(QQuickView::SizeRootObjectToView);
	m_mainWindow->setSource(QUrl("qrc:/qt/qml/TryAlgebra/main.qml"));
	m_mainWindow->setVulkanInstance(m_vulkanInst.get());
	m_mainWindow->setMinimumHeight(300);
	m_mainWindow->setMinimumWidth(300);

	QObject::connect(m_mainWindow.get(), &QQuickWindow::sceneGraphInitialized,
		[this]() {
			auto* ri = m_mainWindow->rendererInterface();

			VkPhysicalDevice physicalDevice =
				*static_cast<VkPhysicalDevice*>(
					ri->getResource(m_mainWindow.get(),
						QSGRendererInterface::PhysicalDeviceResource));

			VkDevice device =
				*static_cast<VkDevice*>(
					ri->getResource(m_mainWindow.get(),
						QSGRendererInterface::DeviceResource));


			FVulkanStatic::InitContext(
				m_mainWindow->vulkanInstance()->vkInstance(),
				physicalDevice,
				device);
			generateMeAtlas();
		});

	QObject::connect(m_mainWindow.get(), &QQuickWindow::sceneGraphInvalidated,
		[]() {
			FVulkanStatic::ClearContext();
		});
	m_mainWindow->showMaximized();
	Actions::compile();
}

FFreeTypeWrap* Application::getFreeTypeWrap()
{
	return &m_ft_wrap;
}

FilesControl* Application::getFilesControl()
{
	return m_files_control;
}

void Application::setQmlEngine(QQmlEngine* engine)
{
	m_qmlEngine = engine;
}

void Application::generateMeAtlas()
{
	MeInfoGenerator atlas_gen;
	MeListModel* meListModel = m_app_model->meListModel();
	atlas_gen.gen(meListModel);
	auto imageProvider = new ImageProvider;
	imageProvider->setImage(meListModel->image());
	m_qmlEngine->addImageProvider(QLatin1String("MeAtlas"), imageProvider);
}

MenuControl* Application::getMenu()
{
	return m_menu_control;
}


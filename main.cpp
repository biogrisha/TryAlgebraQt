
#include <QGuiApplication>
#include <QtQuick/QQuickView>
#include <QQmlApplicationEngine>
#include <QVulkanInstance>
#include <Application/Application.h>
#include <QDebug>
#include <VulkanContext.h>
#include <QQuickGraphicsConfiguration>
#include <vulkan/vulkan.h>
int main(int argc, char* argv[])
{
	//system("cmd /c \"cd /d D:/Projects/TryAlgebraQt/TryAlgebraQt/ThirdParty/MathDocumentRendering/MdRenderingAssets/Shader/ && compile.bat\"");

	QGuiApplication app(argc, argv);
	QVulkanInstance inst;
	inst.setApiVersion(QVersionNumber(1, 4));
	inst.setLayers({
	"VK_LAYER_KHRONOS_validation"
		});
	bool ok = inst.create();
	if (!ok) {
		return 1;
	}
	qDebug() << "vulkan created";
	// This example needs Vulkan. It will not run otherwise.
	QQuickWindow::setGraphicsApi(QSGRendererInterface::Vulkan);

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

	QQuickView view;
	view.setGraphicsConfiguration(config);
	Application* userApplication = new Application(&app);
	qmlRegisterSingletonInstance("com.Application", 1, 0, "UserApplication", userApplication);

	view.setResizeMode(QQuickView::SizeRootObjectToView);
	view.setSource(QUrl("qrc:/qt/qml/TryAlgebra/main.qml"));
	view.setVulkanInstance(&inst);
	QObject::connect(&view, &QQuickWindow::sceneGraphInitialized,
		[&view, userApplication]() {
			auto* ri = view.rendererInterface();

			VkPhysicalDevice physicalDevice =
				*static_cast<VkPhysicalDevice*>(
					ri->getResource(&view,
						QSGRendererInterface::PhysicalDeviceResource));

			VkDevice device =
				*static_cast<VkDevice*>(
					ri->getResource(&view,
						QSGRendererInterface::DeviceResource));


			FVulkanStatic::InitContext(
				view.vulkanInstance()->vkInstance(),
				physicalDevice,
				device);
			userApplication->generateMeAtlas(view.engine());
		});

	QObject::connect(&view, &QQuickWindow::sceneGraphInvalidated,
		[]() {
			FVulkanStatic::ClearContext();
		});
	view.showMaximized();
	return app.exec();
}
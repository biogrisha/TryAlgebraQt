
#include <QGuiApplication>
#include <QtQuick/QQuickView>
#include <QQmlApplicationEngine>
#include <QVulkanInstance>
#include <Application/Application.h>
#include <QDebug>
#include <VulkanContext.h>
#include <QQuickGraphicsConfiguration>
#include <vulkan/vulkan.h>
#include <QQuickItem>



int main(int argc, char* argv[])
{
	//system("cmd /c \"cd /d D:/Projects/TryAlgebraQt/TryAlgebraQt/ThirdParty/MathDocumentRendering/MdRenderingAssets/Shader/ && compile.bat\"");
	//system("cmd /c \"cd /d C:/dev/TryAlgebraQt/ThirdParty/MathDocumentRendering/MdRenderingAssets/Shader/ && compile.bat\"");


	QGuiApplication app(argc, argv);
	QCoreApplication::setOrganizationName("TryAlgebra");
	QCoreApplication::setApplicationName("TryAlgebra");
	QQuickWindow::setGraphicsApi(QSGRendererInterface::Vulkan);


	std::unique_ptr<Application> userApplication = std::make_unique<Application>();
	qmlRegisterSingletonInstance("com.Application", 1, 0, "UserApplication", userApplication.get());

	QQmlEngine engine;
	userApplication->setQmlEngine(&engine);

	QQuickView openProjectDialog(&engine, nullptr);

	openProjectDialog.setResizeMode(QQuickView::SizeViewToRootObject);
	openProjectDialog.setColor(Qt::transparent);
	openProjectDialog.setFlags(
		Qt::Window |
		Qt::FramelessWindowHint
	);

	openProjectDialog.setSource(
		QUrl("qrc:/qt/qml/com/Application/OpenProjectDialog.qml")
	);


	// Center on the primary screen
	QScreen* screen = QGuiApplication::primaryScreen();
	QRect screenGeometry = screen->availableGeometry();

	openProjectDialog.setPosition(
		screenGeometry.center() - QPoint(openProjectDialog.width() / 2, openProjectDialog.height() / 2)
	);

	openProjectDialog.show();
	QQuickItem* root = openProjectDialog.rootObject();
	auto result = app.exec();
	userApplication.reset();
	return result;
}
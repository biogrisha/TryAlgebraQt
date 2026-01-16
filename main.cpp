
#include <QGuiApplication>
#include <QtQuick/QQuickView>
#include <QQmlApplicationEngine>
#include <QVulkanInstance>
#include <Application/Application.h>
#include <QDebug>

int main(int argc, char* argv[])
{
    //system("cmd /c \"cd /d D:\\Projects\\TryAlgebraQt\\TryAlgebraQt\\ThirdParty\\Shader && compile.bat\"");

    QGuiApplication app(argc, argv);
    QVulkanInstance inst;
    // Enable validation layer, if supported. Messages go to qDebug by default.
    inst.setLayers({ "VK_LAYER_KHRONOS_validation" });

    bool ok = inst.create();
    if (!ok) {
        return 1;
    }
    qDebug() << "vulkan created";

    // This example needs Vulkan. It will not run otherwise.
    QQuickWindow::setGraphicsApi(QSGRendererInterface::Vulkan);

    Application* userApplication = new Application(&app);
    qmlRegisterSingletonInstance("com.Application", 1, 0, "UserApplication", userApplication);
    qDebug() << "registred app successfully";
    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:/qt/qml/TryAlgebra/main.qml"));
    view.setVulkanInstance(&inst);
    view.showMaximized();
    return app.exec();
}
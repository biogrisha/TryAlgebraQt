
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

    if (!inst.layers().contains("VK_LAYER_KHRONOS_validation")) {
        return 1;
    }

    // This example needs Vulkan. It will not run otherwise.
    QQuickWindow::setGraphicsApi(QSGRendererInterface::Vulkan);

    QQmlApplicationEngine engine;
    Application* userApplication = new Application(&app);
    qmlRegisterSingletonInstance("com.Application", 1, 0, "UserApplication", userApplication);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() {
            qDebug() << "failed to load";
            QCoreApplication::exit(-1); 
        },
        Qt::QueuedConnection);
    engine.loadFromModule("TryAlgebra", "Main");
    if (auto* window = qobject_cast<QQuickWindow*>(engine.rootObjects().first()))
    {
        window->setVulkanInstance(&inst);
    }
    return app.exec();
}
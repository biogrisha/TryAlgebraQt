
#include <QGuiApplication>
#include <QtQuick/QQuickView>
#include <QVulkanInstance>

int main(int argc, char* argv[])
{
    system("cmd /c \"cd /d D:\\Projects\\TryAlgebraQt\\TryAlgebraQt\\ThirdParty\\Shader && compile.bat\"");
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

    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:///tryAlgebra/Main.qml"));
    view.setVulkanInstance(&inst);
    view.show();

    return app.exec();
}
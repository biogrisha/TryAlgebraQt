// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QGuiApplication>
#include <QtQuick/QQuickView>
#include <QVulkanInstance>
int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    // This example needs Vulkan. It will not run otherwise.
    QQuickWindow::setGraphicsApi(QSGRendererInterface::Vulkan);
    QVulkanInstance inst;
    // Enable validation layer, if supported. Messages go to qDebug by default.
    if (inst.supportedLayers().contains("VK_LAYER_KHRONOS_validation")) {
        inst.setLayers({ "VK_LAYER_KHRONOS_validation" });
    }
    
    if (!inst.create())
        return 1;
    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:///scenegraph/vulkanunderqml/main.qml"));
    view.setVulkanInstance(&inst);
    view.show();

    return app.exec();
}

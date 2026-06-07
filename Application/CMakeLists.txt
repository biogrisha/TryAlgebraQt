# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

qt_add_library(Application STATIC)

find_package(glm REQUIRED)

qt_add_qml_module(Application
    URI com.Application
    VERSION 1.0
    QML_FILES
        MathDocumentWrap.qml
    SOURCES
        AppGlobal.h
        Application.h
        Application.cpp
        MathDocumentCanvas.h
        MathDocumentCanvas.cpp
        Controls/MenuControl.h
        Controls/MenuControl.cpp
        Controls/DocumentControl.h
        Controls/DocumentControl.cpp
        Controls/TabsControl.h
        Controls/TabsControl.cpp
        Controls/FilesControl.h
        Controls/FilesControl.cpp
        Models/MeListModel.h
        Models/MeListModel.cpp
        Models/DocumentsModel.h
        Models/DocumentsModel.cpp
        Models/ApplicationModel.h
        Models/ApplicationModel.cpp
        Modules/MeInfoGenerator.h
        Modules/MeInfoGenerator.cpp
        Modules/ImageProvider.h
        Modules/ImageProvider.cpp
)

target_link_libraries(Application
    PRIVATE 
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    glm::glm
    VulkanFramework
    PUBLIC
    MathDocumentRendering
    TryAlgebraCore
)

target_include_directories(Application BEFORE PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Controls
    ${CMAKE_CURRENT_SOURCE_DIR}/Models
)

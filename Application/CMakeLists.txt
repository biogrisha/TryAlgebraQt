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
        MathDocument.h
        MathDocument.cpp
        Controls/MenuControl.h
        Controls/MenuControl.cpp
        Controls/DocumentControl.h
        Controls/DocumentControl.cpp
        MathEditor/CursorComponentGenerator.h
        MathEditor/MathElementGenerator.h
        MathEditor/MathElementGenerator.cpp
    RESOURCES
        Shaders/DrawEditorLayout.spv
)

target_link_libraries(Application
    PRIVATE 
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    glm::glm
    VulkanFramework
    MathDocumentRendering
    TryAlgebraCore
)

target_include_directories(Application PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/Controls
)
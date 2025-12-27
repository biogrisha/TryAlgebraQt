# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

qt_add_library(Application STATIC)
qt_add_qml_module(Application
    URI com.Application
    VERSION 1.0
    QML_FILES
        MathDocumentWrap.qml
    SOURCES
        AppGlobal.h
        Application.h
        Application.cpp
        Controls/MenuControl.h
        Controls/MenuControl.cpp
)

target_link_libraries(Application
    PRIVATE 
    Qt6::Quick
    TryAlgebraCore
)

target_include_directories(Application PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/Controls
)
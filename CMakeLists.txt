# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(vulkanunderqml LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)
find_package(glm REQUIRED)

add_subdirectory(ThirdParty/VulkanFramework)
add_subdirectory(ThirdParty/MyRTTI)
add_subdirectory(ThirdParty/MathDocumentRendering)
add_subdirectory(ThirdParty/MathLibrary)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(vulkanunderqml
    main.cpp
    vulkansquircle.cpp vulkansquircle.h
)

qt_add_qml_module(vulkanunderqml
    URI VulkanUnderQML
    QML_FILES
        main.qml
    RESOURCES
        squircle.frag.spv
        squircle.vert.spv
    RESOURCE_PREFIX /scenegraph/vulkanunderqml
    NO_RESOURCE_TARGET_PATH
)

target_link_libraries(vulkanunderqml PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    glm::glm
    VulkanFramework
)




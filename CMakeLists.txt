# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(X_VCPKG_APPLOCAL_DEPS_INSTALL ON)
project(TryAlgebraProj LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

qt_standard_project_setup(REQUIRES 6.5)

add_subdirectory(ThirdParty/VulkanFramework)
add_subdirectory(ThirdParty/MyRTTI)
add_subdirectory(ThirdParty/MathDocumentRendering)
add_subdirectory(ThirdParty/MathLibrary)
add_subdirectory(ThirdParty/TryAlgebraCore)
add_subdirectory(Application)


qt_add_executable(TryAlgebra
    main.cpp
)

qt_add_qml_module(TryAlgebra
    URI TryAlgebra
    QML_FILES
        Main.qml
)

set_target_properties(TryAlgebra PROPERTIES
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(TryAlgebra PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    PUBLIC
    Applicationplugin
    Application
)

include(GNUInstallDirs)
install(TARGETS TryAlgebra
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
cmake_minimum_required(VERSION 3.5)

project(Qt_Test VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
find_package(Qt6 REQUIRED COMPONENTS Gui)
find_package(Qt6 REQUIRED COMPONENTS Network)
find_package(Qt6 REQUIRED COMPONENTS SerialPort)
find_package(Qt6 REQUIRED COMPONENTS Concurrent)

set(PROJECT_SOURCES
        main.cpp
        window.cpp
        window.h
        glwidget.cpp
        glwidget.h
        nesemulator.cpp
        nesemulator.h
        cpu.cpp
        cpu.h
        romloader.cpp
        romloader.h
        bus.cpp
        bus.h
        ppu.cpp
        ppu.h
        defs.h
        forward.h
        badge.h
        weather.cpp
        weather.h
        lights.cpp
        lights.h
        resource.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Qt_Test
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Qt_Test APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Qt_Test SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Qt_Test
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Qt_Test PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(Qt_Test PRIVATE Qt6::OpenGLWidgets)
target_link_libraries(Qt_Test PRIVATE Qt6::Gui)
target_link_libraries(Qt_Test PRIVATE Qt6::Network)
target_link_libraries(Qt_Test PRIVATE Qt6::SerialPort)
target_link_libraries(Qt_Test PRIVATE Qt6::Concurrent)

set_target_properties(Qt_Test PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS Qt_Test
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Qt_Test)
endif()

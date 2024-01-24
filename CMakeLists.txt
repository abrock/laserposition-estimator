cmake_minimum_required(VERSION 3.14)

project(laserposition-estimator VERSION 0.1 LANGUAGES CXX)

include_directories("/usr/local/include/aravis-0.10/")

find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED glib-2.0)
pkg_check_modules(GIO IMPORTED_TARGET gio-2.0)
pkg_check_modules(GLIB IMPORTED_TARGET glib-2.0)
pkg_check_modules(GOBJECT IMPORTED_TARGET gobject-2.0)
pkg_check_modules(GTHREAD IMPORTED_TARGET gthread-2.0)
pkg_check_modules(LIBXML IMPORTED_TARGET libxml-2.0)
pkg_check_modules(jsoncpp REQUIRED jsoncpp)
include_directories(${GLIB_INCLUDE_DIRS})

find_package(glog REQUIRED)
find_package(fmt REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick)

set(PROJECT_SOURCES
        main.cpp

        cameramanager.cpp
        misc.cpp

        qml.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(laserposition-estimator
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET laserposition-estimator APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(laserposition-estimator SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(laserposition-estimator
          ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(laserposition-estimator
  PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Quick
)

target_link_libraries(laserposition-estimator
  PUBLIC
  libaravis-0.10.so
  glib-2.0 gio-2.0 gobject-2.0 gthread-2.0
  glog::glog
  fmt::fmt
)

set_target_properties(laserposition-estimator PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS laserposition-estimator
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(laserposition-estimator)
    qt_finalize_executable(laserposition-estimator)
endif()
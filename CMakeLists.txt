cmake_minimum_required(VERSION 3.14)

project(laserposition-estimator VERSION 0.1 LANGUAGES CXX)

include_directories("/usr/local/include/aravis-0.8/")

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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

find_package(Qt5DBus CONFIG REQUIRED)
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Quick CONFIG REQUIRED)
find_package(Qt5Core CONFIG REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick REQUIRED)
include_directories(${Qt5Widgets_INCLUDE_DIRS})

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(Boost COMPONENTS system filesystem iostreams REQUIRED)

find_package(RunningStats REQUIRED)
include_directories(${RUNNINGSTATS_INCLUDE_DIRS})

find_package(GTest REQUIRED)

set(COMMON_LIBS
    glog::glog
    ${RUNNINGSTATS_LIBRARIES}
    ${OpenCV_LIBRARIES}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Widgets
    fmt::fmt
    libaravis-0.8.so
    glib-2.0 gio-2.0 gobject-2.0 gthread-2.0
)

set(LIB_SOURCES
    cameramanager.cpp
    spotfinder.cpp
    misc.cpp
)

set(PROJECT_SOURCES
        main.cpp
        qml.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(laserposition-estimator
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${LIB_SOURCES}
    )
else()
    add_executable(laserposition-estimator
        ${PROJECT_SOURCES}
        ${LIB_SOURCES}
    )
    add_executable(tests tests.cpp
        ${LIB_SOURCES}
        test-spotfinders.cpp
    )
endif()

target_link_libraries(tests
    GTest::gtest
    ${COMMON_LIBS}
)


target_link_libraries(laserposition-estimator
  ${COMMON_LIBS}
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

    qt_import_qml_plugins(tests)
    qt_finalize_executable(tests)
endif()

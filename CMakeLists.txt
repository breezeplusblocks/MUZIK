cmake_minimum_required(VERSION 3.5)

project(MUZIK VERSION 0.1 LANGUAGES CXX)

#set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

enable_testing()

add_subdirectory(third_party/googletest)

add_subdirectory(test)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Multimedia Test REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Multimedia Test REQUIRED)

file(GLOB Headers "./include/*.h")
file(GLOB Sources "./src/*.cpp")
file(GLOB Forms "./form/*.ui")

set(PROJECT_SOURCES
        main.cpp
        ${Headers}
        ${Sources}
        ${Forms}
)

add_library(example include/example.h src/example.cpp)

include_directories(/opt/lib/ffmpeg/include)

link_directories(/opt/lib/ffmpeg/lib)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MUZIK
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Demo APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(MUZIK SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(MUZIK
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(MUZIK PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia Qt${QT_VERSION_MAJOR}::Test -lavcodec -lavformat -lavutil -lswresample)

set_target_properties(MUZIK PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MUZIK)
endif()

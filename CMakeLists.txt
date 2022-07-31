cmake_minimum_required(VERSION 3.5)

project(qaudiotest VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        FileType.cpp
        MemoryMappedFile.cpp
        IffContainer.cpp
        Iff8svx.cpp
        RiffWave.cpp
        RiffContainer.cpp
        IffAiff.cpp
        ieee.c
        DeviceCaps.cpp
        IffMaud.cpp
        Maestro.cpp
        AnsiFile.cpp
        mainwindow.ui
        DeviceCaps.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qaudiotest
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
else()
        add_executable(qaudiotest
            ${PROJECT_SOURCES}
        )
endif()

target_link_libraries(qaudiotest PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)


if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(qaudiotest)
endif()

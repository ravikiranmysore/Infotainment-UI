cmake_minimum_required(VERSION 3.16)
project(InfoDrive VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 QUIET COMPONENTS Core Gui Widgets)
if (NOT Qt6_FOUND)
    find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
    set(QT_TARGET Qt5::Core Qt5::Gui Qt5::Widgets)
else()
    set(QT_TARGET Qt6::Core Qt6::Gui Qt6::Widgets)
endif()

set(SOURCES
    src/main.cpp
    src/HMIWindow.cpp
    src/TopBar.cpp
    src/SideBar.cpp
    src/NotificationOverlay.cpp
    src/HomeScreen.cpp
    src/MediaScreen.cpp
    src/NavigationScreen.cpp
    src/ClimateScreen.cpp
    src/PhoneScreen.cpp
    src/AppsScreen.cpp
    src/SettingsScreen.cpp
)

set(HEADERS
    src/HMIWindow.h
    src/TopBar.h
    src/SideBar.h
    src/NotificationOverlay.h
    src/HomeScreen.h
    src/MediaScreen.h
    src/NavigationScreen.h
    src/ClimateScreen.h
    src/PhoneScreen.h
    src/AppsScreen.h
    src/SettingsScreen.h
)

add_executable(InfoDrive ${SOURCES} ${HEADERS})
target_link_libraries(InfoDrive ${QT_TARGET})
target_include_directories(InfoDrive PRIVATE src)

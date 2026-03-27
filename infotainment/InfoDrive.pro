QT += core gui widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET   = InfoDrive
TEMPLATE = app
CONFIG  += c++17

SOURCES += \
    src/main.cpp           \
    src/HMIWindow.cpp      \
    src/TopBar.cpp         \
    src/SideBar.cpp        \
    src/NotificationOverlay.cpp \
    src/HomeScreen.cpp     \
    src/MediaScreen.cpp    \
    src/NavigationScreen.cpp \
    src/ClimateScreen.cpp  \
    src/PhoneScreen.cpp    \
    src/AppsScreen.cpp     \
    src/SettingsScreen.cpp

HEADERS += \
    src/HMIWindow.h        \
    src/TopBar.h           \
    src/SideBar.h          \
    src/NotificationOverlay.h \
    src/HomeScreen.h       \
    src/MediaScreen.h      \
    src/NavigationScreen.h \
    src/ClimateScreen.h    \
    src/PhoneScreen.h      \
    src/AppsScreen.h       \
    src/SettingsScreen.h

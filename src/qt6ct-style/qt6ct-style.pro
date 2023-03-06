include(../../qt6ct.pri)

TEMPLATE = lib
TARGET = qt6ct-style
QT += widgets

# Input

CONFIG += plugin

target.path = $$PLUGINDIR/styles
INSTALLS += target

INCLUDEPATH += ../

HEADERS += \
    qt6ctproxystyle.h

SOURCES += \
    plugin.cpp \
    qt6ctproxystyle.cpp

OTHER_FILES += \
    qt6ct.json

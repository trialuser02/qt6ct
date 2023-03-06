include(../../qt6ct.pri)

TEMPLATE = lib
TARGET = qt6ct
CONFIG += plugin

QT += gui-private

SOURCES += \
    main.cpp \
    qt6ctplatformtheme.cpp

!equals(DISABLE_WIDGETS,1) {
   QT += widgets
}

OTHER_FILES += qt6ct.json

INCLUDEPATH += ../

HEADERS += \
    qt6ctplatformtheme.h

target.path = $$PLUGINDIR/platformthemes
INSTALLS += target

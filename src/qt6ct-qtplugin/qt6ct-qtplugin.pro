include(../../qt6ct.pri)

TEMPLATE = lib
TARGET = qt6ct
CONFIG += plugin

QT += gui-private

!equals(DISABLE_DBUS, 1):qtHaveModule(dbus) {
    QT += dbus
    message(D-Bus support: Enabled)
} else {
    message(D-Bus support: Disabled)
}

SOURCES += \
    main.cpp \
    ../qt6ct/qt6ct.cpp \
    qt6ctplatformtheme.cpp

!equals(DISABLE_WIDGETS,1) {
   QT += widgets
}

OTHER_FILES += qt6ct.json

INCLUDEPATH += ../

HEADERS += \
    ../qt6ct/qt6ct.h \
    qt6ctplatformtheme.h

target.path = $$PLUGINDIR/platformthemes
INSTALLS += target

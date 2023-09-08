include(../../qt6ct.pri)

TEMPLATE = lib
TARGET = qt6ct-common
LIBS -= -lqt6ct-common

VER_MAJ = $$system(cat qt6ct.h | grep 'define\\ QT6CT_VERSION_MAJOR' | cut -d '\\ ' -f3)
VER_MIN = $$system(cat qt6ct.h | grep 'define\\ QT6CT_VERSION_MINOR' | cut -d '\\ ' -f3)

# Input

DEFINES += QT6CT_LIBRARY

HEADERS += \
    qt6ct.h

SOURCES += \
    qt6ct.cpp

target.path = $$LIBDIR

INSTALLS += target

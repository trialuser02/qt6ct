#Some conf to redirect intermediate stuff in separate dirs
UI_DIR=./.build/ui/
MOC_DIR=./.build/moc/
OBJECTS_DIR=./.build/obj
RCC_DIR=./.build/rcc

QMAKE_DISTCLEAN += -r .build
QMAKE_DISTCLEAN += translations/*.qm

CONFIG += hide_symbols c++17 ordered
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000 QT_DEPRECATED_WARNINGS_SINCE=0x060200
DEFINES += QT_NO_CAST_FROM_BYTEARRAY QT_STRICT_ITERATORS QT_NO_FOREACH
QMAKE_DISTCLEAN += -r .build

#*-g++ {
#  QMAKE_CXXFLAGS += -Werror=suggest-override
#  QMAKE_CXXFLAGS += -Wzero-as-null-pointer-constant
#}

INCLUDEPATH += ../qt6ct-common
QMAKE_LIBDIR += ../qt6ct-common
LIBS += -lqt6ct-common


!isEqual (QT_MAJOR_VERSION, 6) {
  error("Use Qt 6.0.0 or higher.")
}

#Install paths
unix {
  isEmpty(PREFIX): PREFIX = /usr
  isEmpty(PLUGINDIR): PLUGINDIR = $$[QT_INSTALL_PLUGINS]
  isEmpty(LIBDIR): LIBDIR=$$[QT_INSTALL_LIBS]

  BINDIR = $$PREFIX/bin
  DATADIR = $$PREFIX/share

  DEFINES += QT6CT_DATADIR=\\\"$$DATADIR\\\"
}

#Some conf to redirect intermediate stuff in separate dirs
UI_DIR=./.build/ui/
MOC_DIR=./.build/moc/
OBJECTS_DIR=./.build/obj
RCC_DIR=./.build/rcc

QMAKE_DISTCLEAN += -r .build
QMAKE_DISTCLEAN += translations/*.qm

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS QT_DISABLE_DEPRECATED_BEFORE=0x060000

!isEqual (QT_MAJOR_VERSION, 6) {
  error("Use Qt 6.0.0 or higher.")
}

#Install paths
unix {
  isEmpty(PREFIX) {
    PREFIX = /usr
  }
  isEmpty(PLUGINDIR) {
    PLUGINDIR = $$[QT_INSTALL_PLUGINS]
  }

  BINDIR = $$PREFIX/bin
  DATADIR = $$PREFIX/share

  DEFINES += QT6CT_DATADIR=\\\"$$DATADIR\\\"
}

TEMPLATE = subdirs

SUBDIRS += src/qt6ct-qtplugin src/qt6ct-style src/qt6ct


unix:exists($$[QT_INSTALL_BINS]/lrelease){
LRELEASE_EXECUTABLE = $$[QT_INSTALL_BINS]/lrelease
}

unix:exists($$[QT_INSTALL_BINS]/lrelease-qt6){
LRELEASE_EXECUTABLE = $$[QT_INSTALL_BINS]/lrelease-qt6
}


win32:exists($$[QT_INSTALL_BINS]/lrelease.exe){
LRELEASE_EXECUTABLE = $$[QT_INSTALL_BINS]/lrelease.exe
}


isEmpty(LRELEASE_EXECUTABLE){
error(Could not find lrelease executable)
}
else {
message(Found lrelease executable: $$LRELEASE_EXECUTABLE)
}

message(generating translations)
unix:system(find . -name *.ts | xargs $$LRELEASE_EXECUTABLE)
win32:system(for /r %B in (*.ts) do $$LRELEASE_EXECUTABLE %B)

include(qt6ct.pri)

qss.files = qss/*.qss
qss.path = $$DATADIR/qt6ct/qss

colors.files = colors/*.conf
colors.path = $$DATADIR/qt6ct/colors

INSTALLS += qss colors

message (PREFIX=$$PREFIX)
message (BINDIR=$$BINDIR)
message (DATADIR=$$DATADIR)
message (PLUGINDIR=$$PLUGINDIR)
equals (DISABLE_WIDGETS,1):message ("Qt Widgets are disabled!")

include(../../qt6ct.pri)

TEMPLATE = app

QT += widgets gui-private

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    tabpage.cpp \
    appearancepage.cpp \
    fontspage.cpp \
    paletteeditdialog.cpp \
    iconthemepage.cpp \
    interfacepage.cpp \
    fontconfigdialog.cpp \
    qsspage.cpp \
    qsseditordialog.cpp \
    troubleshootingpage.cpp

FORMS += \
    mainwindow.ui \
    appearancepage.ui \
    fontspage.ui \
    paletteeditdialog.ui \
    iconthemepage.ui \
    interfacepage.ui \
    fontconfigdialog.ui \
    previewform.ui \
    qsspage.ui \
    qsseditordialog.ui \
    troubleshootingpage.ui

HEADERS += \
    mainwindow.h \
    tabpage.h \
    appearancepage.h \
    fontspage.h \
    paletteeditdialog.h \
    iconthemepage.h \
    interfacepage.h \
    fontconfigdialog.h \
    qsspage.h \
    qsseditordialog.h \
    troubleshootingpage.h

!equals (DISABLE_WIDGETS,1) {
   DEFINES += USE_WIDGETS
}

RESOURCES = translations/translations.qrc

target.path = $$BINDIR

desktop.files = qt6ct.desktop
desktop.path = $$DATADIR/applications

INSTALLS += target desktop

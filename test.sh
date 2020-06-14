#!/bin/sh

MYDIR=$(dirname $0)

mkdir -p ${MYDIR}/src/qt6ct/styles
ln -s "../../qt6ct-style/libqt6ct-style.so" "${MYDIR}/src/qt6ct/styles/"
#export QT_DEBUG_PLUGINS=1
export QT_LOGGING_RULES="qt6ct.debug=true"
export QT_QPA_PLATFORMTHEME=qt6ct
export QT_QPA_PLATFORM_PLUGIN_PATH=${MYDIR}/src/qt6ct-qtplugin
${MYDIR}/src/qt6ct/qt6ct
rm -rf ${MYDIR}/src/qt6ct/styles/

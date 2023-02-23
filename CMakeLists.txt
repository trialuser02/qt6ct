cmake_minimum_required(VERSION 3.16.0)

project(qt6ct LANGUAGES CXX)

include(GNUInstallDirs)

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

ADD_DEFINITIONS(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000 -DUSE_WIDGETS)

find_package(Qt6 COMPONENTS BuildInternals Widgets LinguistTools REQUIRED)

#get_target_property(QT_QTPATHS_EXECUTABLE Qt6::qtpaths IMPORTED_LOCATION)
get_target_property(QT_LRELEASE_EXECUTABLE Qt6::lrelease IMPORTED_LOCATION)

#if(QT_QTPATHS_EXECUTABLE)
#    message(STATUS "Found qtpaths executable: " ${QT_QTPATHS_EXECUTABLE})
#else()
#    message(FATAL_ERROR "Could NOT find qtpaths executable")
#endif()

if(QT_LRELEASE_EXECUTABLE)
    message(STATUS "Found lrelease executable: " ${QT_LRELEASE_EXECUTABLE})
else()
    message(FATAL_ERROR "Could NOT find lrelease executable")
endif()

get_filename_component(QT_QTPATHS_EXECUTABLE ${QT_LRELEASE_EXECUTABLE} DIRECTORY)
set(QT_QTPATHS_EXECUTABLE ${QT_QTPATHS_EXECUTABLE}/qtpaths)

if(EXISTS ${QT_QTPATHS_EXECUTABLE})
    message(STATUS "Found qtpaths executable: " ${QT_QTPATHS_EXECUTABLE})
else()
    message(FATAL_ERROR "Could NOT find qtpaths executable")
endif()

#execute_process(COMMAND ${QT_QTPATHS_EXECUTABLE} -query QT_INSTALL_PLUGINS OUTPUT_VARIABLE PLUGINDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${QT_QTPATHS_EXECUTABLE} --plugin-dir OUTPUT_VARIABLE PLUGINDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "Plugin path: " ${PLUGINDIR})

message(STATUS "Generating translations ...")
execute_process(COMMAND find ${CMAKE_CURRENT_SOURCE_DIR} -name *.ts COMMAND xargs ${QT_LRELEASE_EXECUTABLE} -silent)

add_subdirectory(src/qt6ct)
add_subdirectory(src/qt6ct-qtplugin)
add_subdirectory(src/qt6ct-style)

install(DIRECTORY qss DESTINATION ${CMAKE_INSTALL_DATADIR}/qt6ct)
install(DIRECTORY colors DESTINATION ${CMAKE_INSTALL_DATADIR}/qt6ct)

if(UNIX)
    add_custom_target(distclean @echo cleaning for source distribution)

    add_custom_command(
        COMMENT "distribution clean"
        COMMAND make
        ARGS -C ${CMAKE_CURRENT_BINARY_DIR} clean
        COMMAND find
        ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "CMakeCache.txt" | xargs rm -rf
        COMMAND find
        ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "CMakeFiles" | xargs rm -rf
        COMMAND find
        ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "Makefile" | xargs rm -rf
        COMMAND find
        ARGS ${CMAKE_CURRENT_BINARY_DIR} -name \"*.cmake\" | xargs rm -rf
        COMMAND find
        ARGS ${CMAKE_CURRENT_SOURCE_DIR} -name "*.qm" | xargs rm -rf
        COMMAND find
        ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "*.pc" | xargs rm -rf
        COMMAND find
        ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "*.depends" | xargs rm -rf
        COMMAND find
        ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "*.cxx_parameters" | xargs rm -rf
        COMMAND find
        ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "moc_*.cpp" | xargs rm -rf
        COMMAND find
        ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "qrc_*.cxx" | xargs rm -rf
        COMMAND find
        ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "*_automoc.cpp" | xargs rm -rf
        COMMAND rm
        ARGS -rf ${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt
        COMMAND find
        ARGS ${CMAKE_CURRENT_BINARY_DIR} -name *_automoc.dir | xargs rm -rf
        COMMAND rm
        ARGS -rf ${CMAKE_CURRENT_BINARY_DIR}/doc/html
        TARGET distclean
    )
endif(UNIX)

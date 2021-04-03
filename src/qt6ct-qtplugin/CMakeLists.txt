project(qt6ct-qtplugin)

set(app_SRCS
  main.cpp
  qt6ctplatformtheme.cpp
  ../qt6ct/qt6ct.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

add_library(qt6ct-qtplugin MODULE ${app_SRCS})
set_target_properties(qt6ct-qtplugin PROPERTIES OUTPUT_NAME qt6ct)
target_link_libraries(qt6ct-qtplugin PRIVATE Qt6::Widgets Qt6::GuiPrivate)
install(TARGETS qt6ct-qtplugin DESTINATION ${PLUGINDIR}/platformthemes)

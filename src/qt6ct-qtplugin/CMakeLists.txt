project(qt6ct-qtplugin)

set(app_SRCS
  main.cpp
  qt6ctplatformtheme.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

add_library(qt6ct-qtplugin MODULE ${app_SRCS})
set_target_properties(qt6ct-qtplugin PROPERTIES OUTPUT_NAME qt6ct)
target_link_libraries(qt6ct-qtplugin PRIVATE Qt6::Widgets Qt6::GuiPrivate $<TARGET_NAME_IF_EXISTS:Qt6::QuickControls2> $<TARGET_NAME_IF_EXISTS:KF6::IconThemes> qt6ct-common)
target_compile_definitions(qt6ct-qtplugin PRIVATE $<$<TARGET_EXISTS:KF6::IconThemes>:KF_ICONTHEMES_LIB>)
install(TARGETS qt6ct-qtplugin DESTINATION ${PLUGINDIR}/platformthemes)

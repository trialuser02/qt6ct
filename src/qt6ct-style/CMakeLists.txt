project(qt6ct-style)

add_definitions(-DUSE_WIDGETS)

set(app_SRCS
  plugin.cpp
  qt6ctproxystyle.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

add_library(qt6ct-style MODULE ${app_SRCS})
target_link_libraries(qt6ct-style PRIVATE Qt6::Widgets qt6ct-common)
install(TARGETS qt6ct-style DESTINATION ${PLUGINDIR}/styles)

project(qt6ct-common)

set(app_SRCS
  ../qt6ct/qt6ct.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

add_library(qt6ct-common SHARED ${app_SRCS})
target_link_libraries(qt6ct-common PRIVATE Qt6::Widgets)
install(TARGETS qt6ct-common DESTINATION ${CMAKE_INSTALL_LIBDIR})

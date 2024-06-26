# https://cmake.org/cmake/help/v3.12/command/file.html#glob-recurse
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

qt6_add_resources(QRC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/resources/resources.qrc)

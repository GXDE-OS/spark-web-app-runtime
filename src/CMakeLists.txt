find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Concurrent WebEngineWidgets)
find_package(Dtk6 REQUIRED COMPONENTS Core Gui Widget)

include(src.cmake)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} ${QRC_FILE})

target_include_directories(${PROJECT_NAME} PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::WebEngineWidgets
    Dtk6::Core
    Dtk6::Gui
    Dtk6::Widget
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::WebEngineWidgets
    Dtk6::Core
    Dtk6::Gui
    Dtk6::Widget
)

install(TARGETS ${PROJECT_NAME} DESTINATION /opt/durapps/${PROJECT_NAME}/bin)

file(CREATE_LINK /opt/durapps/${PROJECT_NAME}/bin/${PROJECT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.link SYMBOLIC)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.link DESTINATION ${CMAKE_INSTALL_BINDIR} RENAME ${PROJECT_NAME})

file(CREATE_LINK /opt/durapps/${PROJECT_NAME}/share/${PROJECT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_datadir.link SYMBOLIC)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_datadir.link DESTINATION ${CMAKE_INSTALL_DATADIR} RENAME ${PROJECT_NAME})

qt5_generate_dbus_interface(
    ivyservice.h
    ${IVY_XML_FILE}
    OPTIONS -A
)

qt5_add_dbus_adaptor(
    IVY_DBUS_ADAPTER_FILE
    ${CMAKE_CURRENT_BINARY_DIR}/${IVY_XML_FILE}
        ivyservice.h
        IvyService
        ".mocuic/ivyadaptor"
)

set_property(
    SOURCE
        ${MOCUIC_DIR}/ivyadaptor.cpp
        ${MOCUIC_DIR}/ivyadaptor.h
    PROPERTY
        SKIP_AUTOGEN ON
)

qt5_add_resources(
    RCC_SOURCES
    resources/server.qrc
)

add_executable(
    ivyserver
        ${server_moc}
        ivygraphicsview.cpp
        ivymainwindow.cpp
        ivyservice.cpp
        server.cpp
        ${IVY_DBUS_ADAPTER_FILE}
        ${RCC_SOURCES}
)

set_target_properties(
    ivyserver
    PROPERTIES
        AUTOGEN_BUILD_DIR
            ${MOCUIC_DIR}
        RUNTIME_OUTPUT_DIRECTORY
            ${IVY_BINARY_DIR}
)

target_include_directories(
    ivyserver
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${MOCUIC_DIR}
)

target_link_libraries(
    ivyserver
        Qt5::DBus
        Qt5::Widgets
)

install(
    TARGETS
        ivyserver
    DESTINATION
        bin
)

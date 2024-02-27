project(ivy)
cmake_minimum_required(VERSION 3.5.0)

find_package(
    Qt5
    COMPONENTS
        DBus
        Widgets
    REQUIRED
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(IVY_XML_FILE .mocuic/box.ivy.server.xml)
set(MOCUIC_DIR ${CMAKE_CURRENT_BINARY_DIR}/.mocuic)

qt5_generate_dbus_interface(
    src/ivyservice.h
    ${IVY_XML_FILE}
    OPTIONS
        -A
)
qt5_add_dbus_adaptor(
    IVY_DBUS_ADAPTER_FILE
    ${CMAKE_CURRENT_BINARY_DIR}/${IVY_XML_FILE}
        src/ivyservice.h
        IvyService
        ".mocuic/ivyadaptor"
)
qt5_wrap_cpp(
    SERVICE_WRAPPER
        src/ivyservice.h
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
        src/resources.qrc
)

add_executable(
    ivy
        src/ivy.cpp
        src/ivymainwindow.cpp
        src/ivyscrollarea.cpp
        src/ivyservice.cpp
        ${IVY_DBUS_ADAPTER_FILE}
        ${RCC_SOURCES}
        ${SERVICE_WRAPPER}
)

target_include_directories(
    ivy
    PRIVATE
        ./
        src/
        ${MOCUIC_DIR}
)

set_target_properties(
    ivy
    PROPERTIES
        AUTOGEN_BUILD_DIR
            ${MOCUIC_DIR}
)

target_link_libraries(
    ivy
        Qt5::DBus
        Qt5::Widgets
)

install(
    TARGETS
        ivy
    DESTINATION
        bin
)

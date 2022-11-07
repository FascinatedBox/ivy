find_package(
    Qt5
    REQUIRED
        Core
        DBus
        Widgets
)

set(IVY_XML_FILE .mocuic/box.ivy.server.xml)
set(MOCUIC_DIR ${CMAKE_CURRENT_BINARY_DIR}/.mocuic)

add_subdirectory(src)

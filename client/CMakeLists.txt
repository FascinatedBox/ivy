find_package(
    Qt5
    REQUIRED
        DBus
)

set(MOCUIC_DIR ${CMAKE_CURRENT_BINARY_DIR}/.mocuic)

add_executable(
    ivyclient
        src/client.cpp
)

set_target_properties(
    ivyclient
    PROPERTIES
        AUTOGEN_BUILD_DIR
            ${MOCUIC_DIR}
        RUNTIME_OUTPUT_DIRECTORY
            ${IVY_BINARY_DIR}
)

target_link_libraries(
    ivyclient
        Qt5::DBus
)

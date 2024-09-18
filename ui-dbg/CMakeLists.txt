project(xna-ui-dbg)

file(COPY "${PROJECT_SOURCE_DIR}/resources" DESTINATION "${CMAKE_BINARY_DIR}")

set(CODE_SOURCES
        src/main.cpp
        src/ui/ui_dbg_window.cpp
        src/ui/ui_dbg_window.h
)

add_executable(${PROJECT_NAME} ${CODE_SOURCES})

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        xna-core    # xna-core library
        -static)

set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
project(xna-core)

find_package(RmlUi REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

set(SHELL_SOURCES
        src/shell/Shell.cpp
        src/shell/Shell.h
        src/shell/ShellFileInterface.cpp
        src/shell/ShellFileInterface.h
)

set(BACKENDS_SOURCES
        src/backends/RmlUi_Platform_GLFW.h
        src/backends/RmlUi_Platform_GLFW.cpp
        src/backends/RmlUi_Renderer_GL3.h
        src/backends/RmlUi_Renderer_GL3.cpp
        src/backends/RmlUi_Include_GL3.h
        src/backends/RmlUi_Include_Windows.h
        src/backends/RmlUi_Backend_GLFW_GL3.cpp
        src/backends/RmlUi_Backend.h
)

set(CORE_SOURCES
        src/ui/xna_window.h
        src/ui/xna_window.cpp
        src/ui/xna_app.cpp
        src/ui/xna_app.h
)

add_library(${PROJECT_NAME} STATIC
        ${SHELL_SOURCES}
        ${BACKENDS_SOURCES}
        ${CORE_SOURCES}
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        Shlwapi             # Windows library for shell utility functions
        RmlUi::RmlUi        # RmlUi library
        glfw3               # GLFW library for window creation
        OpenGL::GL          # OpenGL for rendering
)

target_include_directories(xna-core PUBLIC src)
target_include_directories(xna-core PUBLIC src/ui)
target_include_directories(xna-core PUBLIC src/shell)
target_include_directories(xna-core PUBLIC src/backends)
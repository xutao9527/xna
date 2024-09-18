# 设置安装目录
get_property(XXNP_INSTALL_PATH GLOBAL PROPERTY XXNP_INSTALL_PATH)

file(COPY "${CMAKE_SOURCE_DIR}/3rdparty/RmlUi-6.0/Samples/assets" DESTINATION "${CMAKE_BINARY_DIR}")
file(COPY "${CMAKE_SOURCE_DIR}/3rdparty/RmlUi-6.0/Samples/basic" DESTINATION "${CMAKE_BINARY_DIR}")
file(COPY "${CMAKE_SOURCE_DIR}/3rdparty/RmlUi-6.0/Samples/lua_invaders" DESTINATION "${CMAKE_BINARY_DIR}")
file(COPY "${CMAKE_SOURCE_DIR}/3rdparty/Binary-library/widows-x64/lua/" DESTINATION "${CMAKE_BINARY_DIR}")
file(COPY "${CMAKE_SOURCE_DIR}/3rdparty/Binary-library/widows-x64/runtime/" DESTINATION "${CMAKE_BINARY_DIR}")
file(GLOB FREETYPE_DLL_FILES "${CMAKE_SOURCE_DIR}/3rdparty/Binary-library/widows-x64/freetype/*.dll")
foreach(FREETYPE_DLL_FILE ${FREETYPE_DLL_FILES})
    file(COPY "${FREETYPE_DLL_FILE}" DESTINATION "${CMAKE_BINARY_DIR}")
endforeach()

# 检查lunasvg是否已经安装
if (EXISTS ${XXNP_INSTALL_PATH}/lib/liblunasvg.a)
    message(STATUS "******************** lunasvg is already installed ********************")
else ()
    add_subdirectory(lunasvg)
endif ()


# 检查RmlUI是否已经安装
if (EXISTS ${XXNP_INSTALL_PATH}/lib/librmlui.dll.a)
    message(STATUS "******************** rmlui is already installed ********************")
    file(COPY "${XXNP_INSTALL_PATH}/bin/librmlui.dll" DESTINATION "${CMAKE_BINARY_DIR}")
    file(COPY "${XXNP_INSTALL_PATH}/bin/librmlui_debugger.dll" DESTINATION "${CMAKE_BINARY_DIR}")
else ()
    set(BUILD_SHARED_LIBS ON)           # RMLUI动态库
    set(RMLUI_SVG_PLUGIN ON)            # RMLUI插件SVG
    set(RMLUI_LUA_BINDINGS ON)          # RMLUI插件Lua
    set(RMLUI_SAMPLES ON)               # RMLUI库示例程序生成
    set(RMLUI_BACKEND GLFW_GL3)         # RMLUI库绘制引擎
    set(RMLUI_IS_ROOT_PROJECT TRUE)     # RMLUI库绘制引擎
    add_subdirectory(RmlUi-6.0)         # RMLUI库生成
endif ()

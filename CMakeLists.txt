cmake_minimum_required(VERSION 3.15)

cmake_policy(SET CMP0077 NEW)

# 设置项目信息
project("XNA"
        VERSION "1.0.0"
        DESCRIPTION "A C++ network programming library based on the HTML and CSS standards"
        LANGUAGES "C" "CXX")

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 14)

# 判断构建类型
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    # 设置 Debug 版安装目录
    set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/install/Debug")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    # 设置 Release 版安装目录
    set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/install/Release")
endif ()
# 设置安装目录变量(全局)
set_property(GLOBAL PROPERTY XXNP_INSTALL_PATH ${CMAKE_INSTALL_PREFIX})
get_property(XXNP_INSTALL_PATH GLOBAL PROPERTY XXNP_INSTALL_PATH)

# 清空系统默认路径
set(CMAKE_PREFIX_PATH "")
# 设置自定义路径作为首选路径
set(CMAKE_PREFIX_PATH "${XXNP_INSTALL_PATH}")

# 安装第三方库
add_subdirectory(3rdparty)

# 检查第三方库是否安装
if (NOT EXISTS ${XXNP_INSTALL_PATH}/lib/librmlui.dll.a)
    message(STATUS "3rdparty is not installed, please install it !")
    RETURN()
endif ()

# 公用核心库
add_subdirectory(core)
# 客户端ui界面调试
add_subdirectory(ui-dbg)


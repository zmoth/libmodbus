cmake_minimum_required(VERSION 3.10) # CMake版本必须大于等于此

# 通过git获取版本宏
find_package(Git QUIET)

if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_TAG_VERSION
        ERROR_VARIABLE GIT_TAG_ERROR
        OUTPUT_STRIP_TRAILING_WHITESPACE
        TIMEOUT 5
    )
endif()

message(STATUS ${GIT_TAG_VERSION})

# 如果有错误则设置版本号为0.1.0
if(NOT DEFINED GIT_TAG_ERROR OR NOT "${GIT_TAG_ERROR}" STREQUAL "")
    set(GIT_TAG_VERSION "0.1.0")
endif()

string(REPLACE "-" "." PROJECT_VERSION ${GIT_TAG_VERSION})
string(REPLACE "v" "" PROJECT_VERSION ${GIT_TAG_VERSION})
string(REGEX MATCH "^[0-9]+(\\.[0-9]+)*" PROJECT_VERSION ${PROJECT_VERSION})

message(STATUS ${PROJECT_VERSION})

set(PROJECT_DESCRIPTION "实时数据处理框架")
set(PROJECT_HOMEPAGE_URL "https://zmoth.top")

#
# 配置工程信息
#
project(
    libmodbus
    VERSION ${PROJECT_VERSION}
    DESCRIPTION ${PROJECT_DESCRIPTION}
    HOMEPAGE_URL ${PROJECT_HOMEPAGE_URL}
    LANGUAGES C
)

#
# ########### 选项 ###########
#
option(${PROJECT_NAME}_BUILD_EXAMPLES "是否编译例程" ${PROJECT_IS_TOP_LEVEL})
option(${PROJECT_NAME}_BUILD_PLUGINS "是否构建插件" ${PROJECT_IS_TOP_LEVEL})
option(${PROJECT_NAME}_BUILD_TESTING "是否使用测试" ${PROJECT_IS_TOP_LEVEL})
option(${PROJECT_NAME}_BUILD_DEBUG_POSTFIX_D "是否需要添加后缀以区分是debug还是release" ${PROJECT_IS_TOP_LEVEL})
set(${PROJECT_NAME}_BUILD_POLICY "SHARED" CACHE STRING "从以下选项选择编译模式 [SHARED|STATIC|OBJECT]")
set(INSTALL_INCLUDEDIR "include/${PROJECT_NAME}" CACHE STRING "设置头文件路径")
set(${PROJECT_NAME}_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE STRING "设置CMake安装路径")

#
# ########### 配置 ###########
#
set(CMAKE_CXX_STANDARD 11) # 设置C++版本

# 参考: https://cmake.org/cmake/help/latest/variable/CMAKE_EXPORT_COMPILE_COMMANDS.html
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # 生成compile_commands.json

# 添加cmake模块路径
list(APPEND CMAKE_MODULE_PATH ${${PROJECT_NAME}_SOURCE_DIR}/cmake)

include(GNUInstallDirs)

# 如果此项目作为其他项目的第三方库，以下的配置不生效
if(${PROJECT_IS_TOP_LEVEL})
    # 禁止在源目录编译以及修改，可以在不小心在当前目录编译的时候，报错退出，防止污染源代码：
    set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
    set(CMAKE_DISABLE_SOURCE_CHANGES ON)

    # 设置输出路径
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}) # 静态库的文件夹位置
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}) # 动态库的文件夹位置
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}) # 可执行文件的文件夹位置
endif()

# 定义各个版本对应的后缀，例如d，debug版本后缀。
if(${PROJECT_NAME}_BUILD_DEBUG_POSTFIX_D)
    set(CMAKE_DEBUG_POSTFIX "d")
    set(CMAKE_RELEASE_POSTFIX "")
    set(CMAKE_RELWITHDEBINFO_POSTFIX "rd")
    set(CMAKE_MINSIZEREL_POSTFIX "s")
endif()

#
# ########### 信息 ###########
#
message(STATUS "[${PROJECT_NAME}] 项目名称 ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "[${PROJECT_NAME}] C++ compiler ID is: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "[${PROJECT_NAME}] cmake system is: ${CMAKE_SYSTEM_NAME}")
message(STATUS "[${PROJECT_NAME}] install默认安装位置: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "[${PROJECT_NAME}] 此CMakeLists.txt是否是顶层 ${PROJECT_IS_TOP_LEVEL}")
message(STATUS "[${PROJECT_NAME}] 可执行文件的文件夹位置: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

#
# ########### 构建 ###########
#
if(${${PROJECT_NAME}_BUILD_POLICY} STREQUAL "SHARED")
    add_library(${PROJECT_NAME} SHARED)
elseif(${${PROJECT_NAME}_BUILD_POLICY} STREQUAL "STATIC")
    add_library(${PROJECT_NAME} STATIC)
elseif(${${PROJECT_NAME}_BUILD_POLICY} STREQUAL "OBJECT")
    add_library(${PROJECT_NAME} OBJECT) # 通过代码编译
else()
    add_library(${PROJECT_NAME})
endif()

add_subdirectory(src) # 添加源文件

# 添加配置头文件，将CMake中的版本等信息转换为头文件
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/modbus-version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/modbus-version.h"
)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/config.h"
)

# 生成输出头文件
include(GenerateExportHeader)
generate_export_header(
    ${PROJECT_NAME}
    EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/export.h"
)

# 头文件路径
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC $<INSTALL_INTERFACE:${CMAKE_BINARY_DIR}/include>

    # 自动生成的头文件
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}>

    # 所有头文件都希望能够直接调用到
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

# 宏定义
target_compile_definitions(
    ${PROJECT_NAME}
    PRIVATE ${PROJECT_NAME}_EXPORTS
)

# 设置编译器选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()

# 设定工程（通常是库或可执行文件）的输出目录
set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}
)

#
# ########### 例程 ###########
#
if(${PROJECT_NAME}_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

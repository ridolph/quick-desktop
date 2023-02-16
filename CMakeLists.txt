# For VS2019 and Xcode 12+ support.
cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

if(APPLE)
    # fix: Qt5.15在M1平台默认编译arm版本target却链接x86_64的Qt库，导致链接失败
    set(CMAKE_OSX_ARCHITECTURES "x86_64")
endif()

# project通常指的是逻辑上相对独立、完整，能够独立编译的集合
# 一个project可以单独设置名称、版本和语言
# 一般你的项目中某一部分需要有独立的名称/版本/不同的语言，那么就可以放到单独的project中
project(quick-desktop VERSION 0.0.1 LANGUAGES CXX)

# CMAKE_PROJECT_NAME和PROJECT_NAME都表示最近的project的名称
# 区别是只有最顶层的CMakeLists.txt中的project会影响CMAKE_PROJECT_NAME
# add_subdirectory中的project只会影响PROJECT_NAME

#
# 整个项目的全局配置（target无关）
#

# CMake

# 自动添加CMAKE_CURRENT_BINARY_DIR和CMAKE_CURRENT_SOURCE_DIR到include_directories（全局包含目录，所有target生效）
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# https://cmake.org/cmake/help/latest/prop_gbl/GENERATOR_IS_MULTI_CONFIG.html
# 介绍CMake构建类型较好的文章 https://blog.csdn.net/jjjstephen/article/details/122496126
get_property(GENERATOR_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
message(STATUS "is multi config:" ${GENERATOR_IS_MULTI_CONFIG})

if(GENERATOR_IS_MULTI_CONFIG)
    # 限制多配置类型
    set(CMAKE_CONFIGURATION_TYPES "Debug;RelWithDebInfo" CACHE STRING "limit build type" FORCE)
    # 多配置类型在配置时有多个构建类型，不知道具体编译的构建类型，在编译时才知道
else()
    # default RelWithDebInfo
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE RelWithDebInfo)
    endif()
    message(STATUS "[${PROJECT_NAME}] single config:BUILD_TYPE:${CMAKE_BUILD_TYPE}")
endif()

# Compiler set
message(STATUS "[${PROJECT_NAME}] C++ compiler ID is: ${CMAKE_CXX_COMPILER_ID}")
if (MSVC)
    # FFmpeg cannot be compiled natively by MSVC version < 12.0 (2013)
    if(MSVC_VERSION LESS 1800)
        message(FATAL_ERROR "[${PROJECT_NAME}] ERROR: MSVC version is older than 12.0 (2013).")
    endif()

    message(STATUS "[${PROJECT_NAME}] Set Warnings as error")
    # warning level 3 and all warnings as errors
    add_compile_options(/W3 /WX /wd4566)

    # avoid warning C4819
    add_compile_options(-source-charset:utf-8)
    #add_compile_options(/utf-8)

    # ensure we use minimal "windows.h" lib without the crazy min max macros
    add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN)
    
    # disable SAFESEH - avoid "LNK2026: module unsafe"(Qt5.15&&vs2019)     
    add_link_options(/SAFESEH:NO)
endif()

if (NOT MSVC)
    message(STATUS "[${PROJECT_NAME}] Set warnings as error")
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)

    # disable some warning
    add_compile_options(-Wno-nested-anon-types -Wno-c++17-extensions -Wno-overloaded-virtual)
endif()

# QD define

# check arch
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(QD_CPU_ARCH x64)
else()
    set(QD_CPU_ARCH x86)
endif()
message(STATUS "[${PROJECT_NAME}] CPU_ARCH:${QD_CPU_ARCH}")

# Qt

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick)

if(WIN32)
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS WinExtras)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS WinExtras)
endif()
message(STATUS "[${PROJECT_NAME}] Qt version is: ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}")

# Subdirectory

add_subdirectory(src)

# Output dir

function (_get_all_cmake_targets out_var current_dir)
    get_property(targets DIRECTORY ${current_dir} PROPERTY BUILDSYSTEM_TARGETS)
    get_property(subdirs DIRECTORY ${current_dir} PROPERTY SUBDIRECTORIES)

    foreach(subdir ${subdirs})
        _get_all_cmake_targets(subdir_targets ${subdir})
        list(APPEND targets ${subdir_targets})
    endforeach()

    set(${out_var} ${targets} PARENT_SCOPE)
endfunction()

_get_all_cmake_targets(all_targets ${CMAKE_CURRENT_LIST_DIR})
message(STATUS "[${PROJECT_NAME}] all targets: ${all_targets}")

# $<0:> 使用生成器表达式为每个config设置RUNTIME_OUTPUT_DIRECTORY，这样multi config就不会自动追加CMAKE_BUILD_TYPE子目录了
# 1. multi config介绍 https://cmake.org/cmake/help/latest/prop_gbl/GENERATOR_IS_MULTI_CONFIG.html
# 2. multi config在不用表达式生成器时自动追加子目录说明 https://cmake.org/cmake/help/latest/prop_tgt/RUNTIME_OUTPUT_DIRECTORY.html
# 3. 使用表达式生成器禁止multi config自动追加子目录解决方案 https://stackoverflow.com/questions/7747857/in-cmake-how-do-i-work-around-the-debug-and-release-directories-visual-studio-2
# 一般多配置时CMAKE_BUILD_TYPE默认为空，不应该使用此变量，当前项目打包脚本执行CMake时会指定CMAKE_BUILD_TYPE变量
foreach(target ${all_targets})
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/output/${QD_CPU_ARCH}/${CMAKE_BUILD_TYPE}/$<0:>"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/output/${QD_CPU_ARCH}/${CMAKE_BUILD_TYPE}/$<0:>"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/output/${QD_CPU_ARCH}/${CMAKE_BUILD_TYPE}/$<0:>")
endforeach()

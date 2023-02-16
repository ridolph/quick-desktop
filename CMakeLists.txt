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

# 自动添加CMAKE_CURRENT_BINARY_DIR和CMAKE_CURRENT_SOURCE_DIR到include_directories（全局包含目录，所有target生效）
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick)

if(WIN32)
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS WinExtras)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS WinExtras)
endif()

add_subdirectory(src)

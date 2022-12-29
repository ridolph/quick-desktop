# For VS2019 and Xcode 12+ support.
cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

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

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick)

add_subdirectory(src)

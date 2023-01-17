set(UIBASE_NAME "UIBase")

set(UIBASE_SOURCES
  uibase.cpp
  uibase.h
  framelesswindow/framelesswindowhelper.h
  framelesswindow/win/framelesswindowhelper.cpp
  framelesswindow/win/windownativeeventfilter.h
  framelesswindow/win/windownativeeventfilter.cpp
  framelesswindow/win/windownativeeventhandler.h
  framelesswindow/win/windownativeeventhandler.cpp
  uibase.qrc
)

# 为了qml代码编辑器代码提示，通过QML_IMPORT_PATH设置uibase模块目录，注意是qmldir父目录
# https://doc.qt.io/qtcreator/creator-qml-modules-with-plugins.html#importing-qml-modules
set(QML_IMPORT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/.. CACHE STRING "" FORCE)
message(STATUS "QML_IMPORT_PATH: ${QML_IMPORT_PATH}")

add_library(${UIBASE_NAME} STATIC
  ${UIBASE_SOURCES}
)

target_link_libraries(${UIBASE_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick)
if(WIN32)
    target_link_libraries(${UIBASE_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::WinExtras)
endif()

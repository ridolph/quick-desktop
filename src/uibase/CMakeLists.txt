set(UIBASE_NAME "UIBase")

set(UIBASE_SOURCES
  uibase.cpp
  uibase.h
  framelesswindow/framelesswindowhelper.h
  framelesswindow/framelesswindowhelper.cpp
  framelesswindow/windownativeeventfilterwin.h
  framelesswindow/windownativeeventfilterwin.cpp
  uibase.qrc
)

add_library(${UIBASE_NAME} STATIC
  ${UIBASE_SOURCES}
)

target_link_libraries(${UIBASE_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick)
if(WIN32)
    target_link_libraries(${UIBASE_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::WinExtras)
endif()

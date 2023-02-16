#
# 主target定义
#

# 组织源文件
set(PROJECT_SOURCES
        main.cpp
        qml.qrc
)

set(EXAMPLE_NAME "${CMAKE_PROJECT_NAME}_example")

# 声明target
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${EXAMPLE_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ${EXAMPLE_NAME} APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(${EXAMPLE_NAME} SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${EXAMPLE_NAME}
          ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(${EXAMPLE_NAME}
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick UIBase)

set_target_properties(${EXAMPLE_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER companyname.quickdesktop.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(${EXAMPLE_NAME})
    qt_finalize_executable(${EXAMPLE_NAME})
endif()
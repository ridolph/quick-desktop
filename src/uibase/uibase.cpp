#include "uibase.h"

#include <QDebug>
#include <QQmlEngine>

#include "framelesswindow/framelesswindowhelper.h"

void UIBase::initResource(QQmlEngine& engine)
{
    // 使用静态库的方式导出uibase，c++&qml组件在这个函数注册
    // 不是用插件方式，省去了插件安装的步骤、qmldir等

    // 添加qml组件的搜索路径
    // engine.addImportPath("qrc:/");
    // qDebug() << "importPathList:" << engine.importPathList();

    // 静态库中的uibase.qrc资源文件需要手动注册
    // 这样uibase.qrc中的资源文件才会链接到主程序
    Q_INIT_RESOURCE(uibase);

    // 代码静态分析会将@uri注释告知qml编辑器，从而qml编辑器可以找到我们的模块
    // @uri uibase
    static const char* uri = "uibase";

    // 注册c++组件
    qmlRegisterType<FramelessWindowHelper>(uri, 1, 0, "FramelessWindowHelper");

    // 除了在qmldir中，也可以用以下方式注册qml组件
    // @uri uibase
    qmlRegisterType(QUrl("qrc:/framelesswindow/FramelessWindow.qml"), uri, 1, 0, "FramelessWindow");
}

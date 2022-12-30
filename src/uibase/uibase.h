#pragma once

#include <QObject>

class QQmlEngine;
class UIBase : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(UIBase)

public:
    static void initResource(QQmlEngine& engine);
};

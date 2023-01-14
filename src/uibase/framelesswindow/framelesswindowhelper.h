#pragma once
#include <QObject>

#ifdef Q_OS_WIN32
#include <Windows.h>
#endif

class QQuickWindow;
class FramelessWindowHelper : public QObject {
    Q_OBJECT
    Q_PROPERTY(QQuickWindow* target READ target WRITE setTarget FINAL)
    Q_PROPERTY(bool systemShadow WRITE setSystemShadow FINAL)

public:
    explicit FramelessWindowHelper(QObject* parent = nullptr);
    virtual ~FramelessWindowHelper();

    static bool nativeEventFilter(const QByteArray& eventType, void* message, long* result);

    QQuickWindow* target() const;
    void setTarget(QQuickWindow* target);
    void setSystemShadow(bool systemShadow);
    Q_INVOKABLE void targetShowMinimized();

private:
    bool onNativeEventFilter(const QByteArray& eventType, void* message, long* result);
    bool onShowWindowFilter(MSG* msg, long* result);
    bool onNcHitTestFilter(MSG* msg, long* result);
    bool onNcCalcSizeFilter(MSG* msg, long* result);
    bool onGetMinMaxInfoFilter(MSG* msg, long* result);

    void updateWindowStyle();
    QRect nativeAvailableGeometry() const;

private:
    QQuickWindow* m_target = nullptr;
    bool m_systemShadow = false;
};

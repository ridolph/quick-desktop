#pragma once
#include <QObject>
#include <QPointer>
#include <QQuickWindow>

#ifdef Q_OS_WIN32
class WindowNativeEventHandler;
#endif
#ifdef Q_OS_MACOS
Q_FORWARD_DECLARE_OBJC_CLASS(ProxyNSWindowDelegate);
Q_FORWARD_DECLARE_OBJC_CLASS(NSToolbar);
#endif
class FramelessWindowHelper : public QObject {
    Q_OBJECT
    Q_PROPERTY(QQuickWindow* target READ target WRITE setTarget FINAL)
    Q_PROPERTY(bool systemShadow WRITE setSystemShadow FINAL)

public:
    explicit FramelessWindowHelper(QObject* parent = nullptr);
    virtual ~FramelessWindowHelper();

    QQuickWindow* target() const;
    void setTarget(QQuickWindow* target);
    void setSystemShadow(bool systemShadow);

#ifdef Q_OS_WIN32
    Q_INVOKABLE void targetShowMinimized();
#endif

private:
    QPointer<QQuickWindow> m_target;
    bool m_systemShadow = false;

#ifdef Q_OS_WIN32
    WindowNativeEventHandler* m_windowNativeEventHandler = nullptr;
#endif

#ifdef Q_OS_MACOS
    ProxyNSWindowDelegate* m_proxyDelegate = nullptr;
    NSToolbar* m_toolbar = nullptr;
#endif
};

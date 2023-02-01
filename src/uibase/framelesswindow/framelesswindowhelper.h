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
    Q_PROPERTY(Qt::WindowFlags windowFlags READ windowFlags WRITE setWindowFlags NOTIFY windowFlagsChanged FINAL)

public:
    explicit FramelessWindowHelper(QObject* parent = nullptr);
    virtual ~FramelessWindowHelper();

    QQuickWindow* target() const;
    void setTarget(QQuickWindow* target);
    inline void setSystemShadow(bool systemShadow) { m_systemShadow = systemShadow; }
    inline const Qt::WindowFlags& windowFlags() { return m_windowFlags; }
    inline void setWindowFlags(const Qt::WindowFlags& windowFlags) {
        if (m_windowFlags == windowFlags) {
            return;
        }

        m_windowFlags = windowFlags;
        emit windowFlagsChanged(m_windowFlags);
    }

#ifdef Q_OS_WIN32
    Q_INVOKABLE void targetShowMinimized();
#endif

signals:
    void windowFlagsChanged(const Qt::WindowFlags& windowFlags);

private:
    QPointer<QQuickWindow> m_target;
    bool m_systemShadow = false;
    Qt::WindowFlags m_windowFlags = Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowFullscreenButtonHint;

#ifdef Q_OS_WIN32
    WindowNativeEventHandler* m_windowNativeEventHandler = nullptr;
#endif

#ifdef Q_OS_MACOS
    ProxyNSWindowDelegate* m_proxyDelegate = nullptr;
    NSToolbar* m_toolbar = nullptr;
#endif
};

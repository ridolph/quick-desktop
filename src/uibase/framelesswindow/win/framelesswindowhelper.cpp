#include "../framelesswindowhelper.h"

#include "windownativeeventhandler.h"

FramelessWindowHelper::FramelessWindowHelper(QObject* parent)
    : QObject(parent)
{
}

FramelessWindowHelper::~FramelessWindowHelper()
{
    if (m_windowNativeEventHandler) {
        delete m_windowNativeEventHandler;
        m_windowNativeEventHandler = nullptr;
    }
}

QQuickWindow* FramelessWindowHelper::target() const
{
    return m_target;
}

void FramelessWindowHelper::setTarget(QQuickWindow* target)
{
    // m_target能且仅能设置一次非空值
    Q_ASSERT(target);
    Q_ASSERT(!m_target);

    m_target = target;
    m_windowNativeEventHandler = new WindowNativeEventHandler(this, m_systemShadow);
}

void FramelessWindowHelper::setSystemShadow(bool systemShadow)
{
    m_systemShadow = systemShadow;
}

void FramelessWindowHelper::targetShowMinimized()
{
    if (!m_target) {
        return;
    }

    // 由于Qt官方bug， windows平台最大化-最小化-恢复，窗口不能恢复成最大化，需要自己实现最小化
    auto oldStates = m_target->windowStates();
    m_target->setWindowStates((oldStates & ~Qt::WindowActive) | Qt::WindowMinimized);
}

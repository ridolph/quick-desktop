#include "windownativeeventfilterwin.h"

#include <QGuiApplication>

#include "framelesswindowhelper.h"

WindowNativeEventFilterWin::WindowNativeEventFilterWin() { }

WindowNativeEventFilterWin::~WindowNativeEventFilterWin() { }

WindowNativeEventFilterWin& WindowNativeEventFilterWin::instance()
{
    static WindowNativeEventFilterWin s_WindowNativeEventFilterWin;
    return s_WindowNativeEventFilterWin;
}

void WindowNativeEventFilterWin::init()
{
    if (m_inited) {
        return;
    }

    m_inited = true;
    QGuiApplication::instance()->installNativeEventFilter(this);
}

bool WindowNativeEventFilterWin::nativeEventFilter(const QByteArray& eventType, void* message, long* result)
{
    return FramelessWindowHelper::nativeEventFilter(eventType, message, result);
}

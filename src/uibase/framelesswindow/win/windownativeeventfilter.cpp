#include "windownativeeventfilter.h"

#include <QGuiApplication>

#include "windownativeeventhandler.h"

WindowNativeEventFilter::WindowNativeEventFilter() { }

WindowNativeEventFilter::~WindowNativeEventFilter() { }

WindowNativeEventFilter& WindowNativeEventFilter::instance()
{
    static WindowNativeEventFilter s_WindowNativeEventFilterWin;
    return s_WindowNativeEventFilterWin;
}

void WindowNativeEventFilter::init()
{
    if (m_inited) {
        return;
    }

    m_inited = true;
    QGuiApplication::instance()->installNativeEventFilter(this);
}

bool WindowNativeEventFilter::nativeEventFilter(const QByteArray& eventType, void* message, long* result)
{
    return WindowNativeEventHandler::nativeEventFilter(eventType, message, result);
}

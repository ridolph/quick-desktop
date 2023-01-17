#pragma once

#include <Windows.h>
#include <WinUser.h>

#include <QObject>

class FramelessWindowHelper;
class WindowNativeEventHandler {
public:
    WindowNativeEventHandler(FramelessWindowHelper* framelessWindowHelper, bool systemShadow);
    ~WindowNativeEventHandler();

    static bool nativeEventFilter(const QByteArray& eventType, void* message, long* result);

private:
    bool onNativeEventFilter(const QByteArray& eventType, void* message, long* result);
    bool onShowWindowFilter(MSG* msg, long* result);
    bool onNcHitTestFilter(MSG* msg, long* result);
    bool onNcCalcSizeFilter(MSG* msg, long* result);
    bool onGetMinMaxInfoFilter(MSG* msg, long* result);

    void updateWindowStyle();
    QRect nativeAvailableGeometry() const;

private:
    FramelessWindowHelper* m_framelessWindowHelper = nullptr;
};

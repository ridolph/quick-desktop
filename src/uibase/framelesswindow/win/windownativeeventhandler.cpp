#include "windownativeeventhandler.h"

#include <Windowsx.h>

#include <QDebug>
#include <QMap>
#include <QQuickItem>
#include <QQuickWindow>
#include <QScreen>
#include <QtWin>

#include "../framelesswindowhelper.h"
#include "windownativeeventfilter.h"

// 参考：
// https://github.com/barry-ran/FramelessHelper/blob/patch-1/FramelessHelper/Kernels/NativeWindowHelper.cpp
// https://github.com/Bringer-of-Light/Qt-Nice-Frameless-Window/blob/HEAD/framelesswindow/framelesswindow.cpp

static QMap<quint64, WindowNativeEventHandler*> s_FramelessWindowHelperMap;

WindowNativeEventHandler::WindowNativeEventHandler(FramelessWindowHelper* framelessWindowHelper, bool systemShadow)
    : m_framelessWindowHelper(framelessWindowHelper)
{
    WindowNativeEventFilter::instance().init();

    s_FramelessWindowHelperMap[m_framelessWindowHelper->target()->winId()] = this;

    // WS_CAPTION|WS_THICKFRAME导致在两个dpi相同的屏幕间移动窗口时，QML绘制区域缩小
    // SWP_FRAMECHANGED触发WM_NCCALCSIZE消息，进而去除多余的标题栏和标准边框区域
    QObject::connect(m_framelessWindowHelper->target(), &QWindow::screenChanged, m_framelessWindowHelper->target(), [=](QScreen* screen) {
        Q_UNUSED(screen);
        auto hWnd = reinterpret_cast<HWND>(m_framelessWindowHelper->target()->winId());
        ::SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
    });

    updateWindowStyle();

    // 使用系统阴影（和圆角冲突）
    if (QtWin::isCompositionEnabled()) {
        // 相关介绍 https://blog.csdn.net/thanklife/article/details/80108480
        // ::DwmExtendFrameIntoClientArea
        if (systemShadow) {
            QtWin::extendFrameIntoClientArea(m_framelessWindowHelper->target(), 1, 1, 1, 1);
        } else {
            QtWin::extendFrameIntoClientArea(m_framelessWindowHelper->target(), 0, 0, 0, 0);
        }
    }
}

WindowNativeEventHandler::~WindowNativeEventHandler()
{
    s_FramelessWindowHelperMap.remove(m_framelessWindowHelper->target()->winId());
}

bool WindowNativeEventHandler::nativeEventFilter(const QByteArray& eventType, void* message, long* result)
{
    Q_UNUSED(eventType);

    MSG* msg = static_cast<MSG*>(message);
    if (!msg) {
        return false;
    }
    if (!s_FramelessWindowHelperMap.contains(reinterpret_cast<quint64>(msg->hwnd))) {
        return false;
    }

    return s_FramelessWindowHelperMap[reinterpret_cast<quint64>(msg->hwnd)]->onNativeEventFilter(eventType, message, result);
}

bool WindowNativeEventHandler::onNativeEventFilter(const QByteArray& eventType, void* message, long* result)
{
    Q_UNUSED(eventType);
    Q_CHECK_PTR(m_framelessWindowHelper->target());

    MSG* msg = static_cast<MSG*>(message);
    switch (msg->message) {
    case WM_SHOWWINDOW: {
        return onShowWindowFilter(msg, result);
    }
    case WM_NCHITTEST: {
        return onNcHitTestFilter(msg, result);
    }
    case WM_NCCALCSIZE: {
        return onNcCalcSizeFilter(msg, result);
    }
    /*
    case WM_GETMINMAXINFO: {
        return onGetMinMaxInfoFilter(msg, result);
    }
    */
    case WM_DPICHANGED: {
        return false;
    }
    default:
        return false;
    }
}

bool WindowNativeEventHandler::onShowWindowFilter(MSG* msg, long* result)
{
    Q_UNUSED(result);
    bool show = static_cast<bool>(msg->wParam);
    if (show) {
    }
    return false;
}

bool WindowNativeEventHandler::onNcHitTestFilter(MSG* msg, long* result)
{
    // native坐标转换为Qt坐标
    // 参考QHighDpi::fromNativeLocalPosition
    POINT nativePos = { GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam) };
    ::ScreenToClient(msg->hwnd, &nativePos);
    auto dpi = m_framelessWindowHelper->target()->effectiveDevicePixelRatio();
    QPoint localPos { nativePos.x, nativePos.y };
    localPos /= dpi;
    QPoint globalPos = m_framelessWindowHelper->target()->mapToGlobal(localPos);

    // 拖拽移动窗口
    QQuickItem* backgroundShadowItem = m_framelessWindowHelper->target()->findChild<QQuickItem*>("backgroundShadowItem");
    Q_CHECK_PTR(backgroundShadowItem);
    // drag border
    QPoint borderItemPos = backgroundShadowItem->mapFromGlobal(globalPos).toPoint();
    constexpr int dragWidth = 5;
    enum RegionMask {
        Client = 0x0000,
        Top = 0x0001,
        Left = 0x0010,
        Right = 0x0100,
        Bottom = 0x1000,
    };

    auto borderRect = backgroundShadowItem->boundingRect();
    int borderLeftMin = borderRect.left() - dragWidth;
    int borderLeftMax = borderRect.left() + dragWidth;
    int borderTopMin = borderRect.top() - dragWidth;
    int borderTopMax = borderRect.top() + dragWidth;
    int borderRightMin = borderRect.right() - dragWidth;
    int borderRightMax = borderRect.right() + dragWidth;
    int borderBottomMin = borderRect.bottom() - dragWidth;
    int borderBottomMax = borderRect.bottom() + dragWidth;

    auto dragResult = (Top * ((borderItemPos.y() > borderTopMin) && (borderItemPos.y() < borderTopMax)))
        | (Left * ((borderItemPos.x() > borderLeftMin) && (borderItemPos.x() < borderLeftMax)))
        | (Right * ((borderItemPos.x() > borderRightMin) && (borderItemPos.x() < borderRightMax)))
        | (Bottom * ((borderItemPos.y() > borderBottomMin) && (borderItemPos.y() < borderBottomMax)));

    bool wResizable = m_framelessWindowHelper->target()->minimumWidth() < m_framelessWindowHelper->target()->maximumWidth();
    bool hResizable = m_framelessWindowHelper->target()->minimumHeight() < m_framelessWindowHelper->target()->maximumHeight();
    // 最大化/全屏不允许拖拽改变窗口大小
    if ((m_framelessWindowHelper->target()->windowStates() & Qt::WindowMaximized) || (m_framelessWindowHelper->target()->windowStates() & Qt::WindowFullScreen)) {
        wResizable = false;
        hResizable = false;
    }

    switch (dragResult) {
    case Top | Left:
        *result = wResizable && hResizable ? HTTOPLEFT : HTCLIENT;
        return true;
    case Top:
        *result = hResizable ? HTTOP : HTCLIENT;
        return true;
    case Top | Right:
        *result = wResizable && hResizable ? HTTOPRIGHT : HTCLIENT;
        return true;
    case Right:
        *result = wResizable ? HTRIGHT : HTCLIENT;
        return true;
    case Bottom | Right:
        *result = wResizable && hResizable ? HTBOTTOMRIGHT : HTCLIENT;
        return true;
    case Bottom:
        *result = hResizable ? HTBOTTOM : HTCLIENT;
        return true;
    case Bottom | Left:
        *result = wResizable && hResizable ? HTBOTTOMLEFT : HTCLIENT;
        return true;
    case Left:
        *result = wResizable ? HTLEFT : HTCLIENT;
        return true;
    }

    // 拖拽移动窗口
    // 选择不在native中实现移动窗口的原因：
    // 1. 只能设置一个区域，对区域内的事件，哪些给子控件处理，哪些作为移动窗口处理，需要自己判断
    // 2. Qt5.15新增了startSystemMove，移动窗口效果和系统一样（windows snap&mac边缘吸附都有）
#if 0
    // movableArea
    QQuickItem* movableArea = m_framelessWindowHelper->target()->findChild<QQuickItem*>("movableArea");
    Q_CHECK_PTR(movableArea);
    QPoint movablePos = movableArea->mapFromGlobal(globalPos).toPoint();
    // 如果movableArea中有子控件，则不处理拖动
    if (movableArea->contains(movablePos)) {
        // 一开始用独立OpacityMask实现了圆角，会导致childAt会失效&无法响应鼠标事件：
        // 因为独立OpacityMask要求目标Item visible: false，进而会导致Item的childAt都返回nullptr，鼠标事件也无法响应
        // 后来改用layer+OpacityMask的方式既能实现圆角，又不影响鼠标事件
        bool inChild = nullptr != movableArea->childAt(movablePos.x(), movablePos.y());

        /*
        // 自己用childItems+contains实现childAt
        bool inChild = false;
        QList<QQuickItem*> childItems = movableArea->childItems();
        for (const auto& child : childItems) {
            if (child->contains(movableArea->mapToItem(child, movablePos))) {
                inChild = true;
                break;
            }
        }
        */

        if (!inChild) {
            *result = HTCAPTION;
            return true;
        }
    }
#endif
    return false;
}

bool WindowNativeEventHandler::onNcCalcSizeFilter(MSG* msg, long* result)
{
    Q_UNUSED(result);

    // 处理WM_NCCALCSIZE消息来去除标题栏和标准边框
    // https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-nccalcsize#remarks
    if (msg->wParam) {
        if (::IsZoomed(msg->hwnd)) {
            // 修复最大化时，窗口大小超出屏幕
            // https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-nccalcsize_params#members
            NCCALCSIZE_PARAMS* params = reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam);
            // 这里需要native坐标系，不使用qt api是因为qt获取的qt坐标经过了dpi转换，native坐标系直接用native api方便快捷
            QRect geometry = nativeAvailableGeometry();
            params->rgrc[0].top = geometry.top();
            params->rgrc[0].left = geometry.left();
            // 不+1有一个像素白边
            params->rgrc[0].right = geometry.right() + 1;
            params->rgrc[0].bottom = geometry.bottom() + 1;

            *result = 0;
        } else {
            NCCALCSIZE_PARAMS* params = reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam);
            params->rgrc[2] = params->rgrc[1];
            params->rgrc[1] = params->rgrc[0];
            // WVR_REDRAW修复了qml窗口缩小时子控件重绘延时问题
            *result = WVR_REDRAW;
        }

        return true;
    }

    return false;
}

bool WindowNativeEventHandler::onGetMinMaxInfoFilter(MSG* msg, long* result)
{
    return false;
    /*
    // 只处理最大化场景
    if (!::IsZoomed(msg->hwnd)) {
        return false;
    }

    QRect availableGeometry = nativeAvailableGeometry();
    LPMINMAXINFO lpMinMaxInfo = reinterpret_cast<LPMINMAXINFO>(msg->lParam);
    lpMinMaxInfo->ptMaxPosition.x = 0;
    lpMinMaxInfo->ptMaxPosition.y = 0;
    lpMinMaxInfo->ptMaxSize.x = availableGeometry.right() - availableGeometry.left();
    lpMinMaxInfo->ptMaxSize.y = availableGeometry.bottom() - availableGeometry.top();

    lpMinMaxInfo->ptMinTrackSize.x = m_framelessWindowHelper->target()->minimumWidth();
    lpMinMaxInfo->ptMinTrackSize.y = m_framelessWindowHelper->target()->minimumHeight();
    lpMinMaxInfo->ptMaxTrackSize.x = m_framelessWindowHelper->target()->maximumWidth();
    lpMinMaxInfo->ptMaxTrackSize.y = m_framelessWindowHelper->target()->maximumHeight();

    *result = 0;
    return true;
    */
}

void WindowNativeEventHandler::updateWindowStyle()
{
    if (!m_framelessWindowHelper->target()) {
        return;
    }

    // 1. 必须有WS_THICKFRAME和WS_MAXIMIZEBOX才有windows窗口的神奇效果
    // 2. 必须有WS_CAPTION，否则触发半屏神奇效果后，会有显示窗口标题栏的bug
    // 3. 但是WS_CAPTION&WS_THICKFRAME会将窗口标题栏和标准边框带回来，需要处理WM_NCCALCSIZE消息来去除标题栏和标准边框区域
    // 4. WS_CAPTION|WS_THICKFRAME还导致在两个dpi相同的屏幕间移动窗口时，qml绘制区域缩小，
    //    原因是窗口在两个屏幕间移动时QML判断窗口有WS_CAPTION|WS_THICKFRAME属性，所以空出了标题栏和框架的区域
    //    而我们处理第3个问题时是通过WM_NCCALCSIZE来去除标题栏和标准边框区域的，在这个场景下没效果是以为没有触发WM_NCCALCSIZE消息
    //    可以通过响应m_target的screenChanged信号来利用SWP_FRAMECHANGED触发WM_NCCALCSIZE消息，进而去除多余的标题栏和标准边框区域
    // 5. 两个dpi不同的屏幕间移动窗口不会有第4个问题，因为屏幕dpi不同所以窗口大小会变化，从而会触发WM_NCCALCSIZE消息，
    //    而我们在WM_NCCALCSIZE消息中去除了多余的标题栏和标准边框区域

    // fixSize优先于Qt::WindowMaximizeButtonHint
    bool fixWidth = m_framelessWindowHelper->target()->minimumWidth() == m_framelessWindowHelper->target()->maximumWidth();
    bool fixHeight = m_framelessWindowHelper->target()->minimumHeight() == m_framelessWindowHelper->target()->maximumHeight();
    LONG newStyle = WS_CAPTION | WS_THICKFRAME;
    if ((m_framelessWindowHelper->target()->flags() & Qt::WindowMaximizeButtonHint) && !fixWidth && !fixHeight) {
        newStyle |= WS_MAXIMIZEBOX;
    }
    if (m_framelessWindowHelper->target()->flags() & Qt::WindowMinimizeButtonHint) {
        newStyle |= WS_MINIMIZEBOX;
    }

    HWND hWnd = reinterpret_cast<HWND>(m_framelessWindowHelper->target()->winId());
    LONG currentStyle = ::GetWindowLong(hWnd, GWL_STYLE);
    ::SetWindowLong(hWnd, GWL_STYLE, currentStyle | newStyle);
}

QRect WindowNativeEventHandler::nativeAvailableGeometry() const
{
    MONITORINFO mi { 0 };
    mi.cbSize = sizeof(MONITORINFO);

    auto hWnd = reinterpret_cast<HWND>(m_framelessWindowHelper->target()->winId());
    auto hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    if (!hMonitor || !GetMonitorInfoW(hMonitor, &mi)) {
        Q_ASSERT(NULL != hMonitor);
        return m_framelessWindowHelper->target()->screen()->availableGeometry();
    }

    return QRect(mi.rcWork.left, mi.rcWork.top, mi.rcWork.right - mi.rcWork.left, mi.rcWork.bottom - mi.rcWork.top);
}

#import "../framelesswindowhelper.h"

#import <Cocoa/Cocoa.h>

#import <QDebug>
#import <QMap>
#import <QQuickItem>
#import <QScreen>

void customTitleBar(NSWindow* window)
{
    if (!window) {
        return;
    }

    // 标题栏透明
    [window setTitlebarAppearsTransparent:YES];
    // 隐藏标题
    [window setTitleVisibility:NSWindowTitleHidden];
    // 关闭系统标题栏拖动，避免与自定义冲突
    [window setMovable:NO];
    [window setMovableByWindowBackground:NO];
    // view覆盖标题栏区域
    window.styleMask |= NSWindowStyleMaskFullSizeContentView;
}

// NSObject 消息转发流程 https://juejin.cn/post/6857013884298133517#heading-2
// oc hook函数实现 https://juejin.cn/post/7008737882479263781
@interface ProxyNSWindowDelegate : NSObject <NSWindowDelegate>
- (instancetype)initWithWindow:(NSWindow*)window;
@end

@implementation ProxyNSWindowDelegate {
    NSObject<NSWindowDelegate>* m_originWindowDelegate;
    NSWindow* m_window;
}

- (instancetype)initWithWindow:(NSWindow*)window {
    self->m_window = window;
    self->m_originWindowDelegate = [window delegate];
    window.delegate = self;

    return self;
}

- (BOOL)respondsToSelector:(SEL)aSelector
{
    // 返回是否实现了指定的方法，一般调用delegate前都用respondsToSelector判断下再调用
    // 当前类的目的是扩展m_originWindowDelegate的功能，想处理的方法在当前类处理，不想处理的转发到m_originWindowDelegate
    // 所以需要同时在当前类和m_originWindowDelegate中判断是否实现了指定方法
    // 当super是NSObject时，respondsToSelector的实现是基于当前类查找selector的实现
    // 当super是NSProxy时，respondsToSelector的实现是被forwardInvocation转发
    // 在当前类forwardInvocation把所有selector都被转发给了m_originWindowDelegate
    // 在m_originWindowDelegate中执行respondsToSelector是肯定找不到我们在当前类新增的方法的
    // https://www.jianshu.com/p/a5d0eaf1eefa
    return [super respondsToSelector:aSelector] || [self->m_originWindowDelegate respondsToSelector:aSelector];
}

// 对于需要动态转发的消息统一交给m_originWindowDelegate处理，当前类只处理当前类实现了的方法
// 当前类直接实现的方法走不到消息转发流程
- (NSMethodSignature *)methodSignatureForSelector:(SEL)aSelector {
    return [self->m_originWindowDelegate methodSignatureForSelector:aSelector];
}

- (void)forwardInvocation:(NSInvocation *)anInvocation {
    // NSLog(@"forwardInvocation:%s", anInvocation.selector);
    [anInvocation invokeWithTarget:self->m_originWindowDelegate];
}

- (NSApplicationPresentationOptions)window:(NSWindow *)window willUseFullScreenPresentationOptions:(NSApplicationPresentationOptions)proposedOptions
{
    // 设置窗口全屏时自动隐藏toolbar
    return (NSApplicationPresentationFullScreen |
        NSApplicationPresentationAutoHideMenuBar |
        NSApplicationPresentationAutoHideToolbar);
}

// 模拟appstore动态调整交通灯高度的效果：
// 1. 窗口非全屏标题栏为大高度（显示工具栏）
// 2. 窗口非全屏标题栏为小高度（隐藏工具栏）
- (void)windowDidEnterFullScreen:(NSNotification *)notification {
    [[self->m_window toolbar] setVisible:NO];
}

- (void)windowWillExitFullScreen:(NSNotification *)notification {
    // fix: 退出全屏后标题栏出现
    customTitleBar(self->m_window);
}

- (void)windowDidExitFullScreen:(NSNotification *)notification {
    // fix: 退出全屏后标题栏出现
    customTitleBar(self->m_window);
}

- (void)windowDidResize:(NSNotification *)notification {
    // 进入全屏时会隐藏toobar，退出全屏尺寸变化时恢复toolbar
    if (![[self->m_window toolbar] isVisible]) {
        [[self->m_window toolbar] setVisible:YES];
    }
}

@end

FramelessWindowHelper::FramelessWindowHelper(QObject* parent)
    : QObject(parent)
{
}

FramelessWindowHelper::~FramelessWindowHelper()
{
    if (m_proxyDelegate) {
        [m_proxyDelegate release];
        m_proxyDelegate = nullptr;
    }
    if (m_toolbar) {
        [m_toolbar release];
        m_toolbar = nullptr;
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

    QWindow* win = qobject_cast<QWindow*>(m_target);
    NSView* view = reinterpret_cast<NSView*>(win->winId());
    NSWindow* window = [view window];
    m_proxyDelegate = [[ProxyNSWindowDelegate alloc] initWithWindow: window];

    customTitleBar(window);

    // mac系统窗口的标题栏有三种过高度：
    // 0 小高度（终端窗口），1 中高度（xcode），2 大高度（访达）
    int standardWindowButtonHeightLevel = 2;
    // 默认标题栏是是小高度
    // 增加Toolbar后（本例在xib中添加了一个空的ToolBar），可以通过设置setToolbarStyle调整标题栏另外两种高度：
    // NSWindowToolbarStyleUnified 大高度
    // NSWindowToolbarStyleUnifiedCompact 中高度
    if (0 != standardWindowButtonHeightLevel) {
        m_toolbar = [[NSToolbar alloc] init];
        [window setToolbar:m_toolbar];
    }
    if (1 == standardWindowButtonHeightLevel) {
        [window setToolbarStyle:NSWindowToolbarStyleUnifiedCompact];
    }
    if (2 == standardWindowButtonHeightLevel) {
        [window setToolbarStyle:NSWindowToolbarStyleUnified];
    }
}

void FramelessWindowHelper::setSystemShadow(bool systemShadow) {
    m_systemShadow = systemShadow;
}

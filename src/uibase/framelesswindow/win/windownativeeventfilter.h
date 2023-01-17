#pragma once

#include <QAbstractNativeEventFilter>

class WindowNativeEventFilter : public QAbstractNativeEventFilter {
protected:
    WindowNativeEventFilter();
    ~WindowNativeEventFilter() override;

public:
    static WindowNativeEventFilter& instance();

    void init();

private:
    bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) override;

private:
    bool m_inited = false;
};

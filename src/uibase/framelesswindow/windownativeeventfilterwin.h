#pragma once

#include <QAbstractNativeEventFilter>

class WindowNativeEventFilterWin : public QAbstractNativeEventFilter {
protected:
    WindowNativeEventFilterWin();
    ~WindowNativeEventFilterWin() override;

public:
    static WindowNativeEventFilterWin& instance();

    void init();

private:
    bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) override;

private:
    bool m_inited = false;
};

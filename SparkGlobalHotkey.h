#ifndef SPARKGLOBALHOTKEY_H
#define SPARKGLOBALHOTKEY_H

#include <QObject>
#include <QKeySequence>
#include <QAbstractNativeEventFilter> // 确保包含这个头文件


class SparkGlobalHotkey : public QObject, public QAbstractNativeEventFilter {
    Q_OBJECT

public:
    explicit SparkGlobalHotkey(QObject *parent = nullptr);
    ~SparkGlobalHotkey();

    bool registerHotkey(const QKeySequence &keySequence);

signals:
    void activated();

private:
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override; // 正确签名
    void unregisterHotkey();

    unsigned int keyCode;   // X11 的键码
    unsigned int modifiers; // X11 的修饰符
};

#endif // SPARKGLOBALHOTKEY_H
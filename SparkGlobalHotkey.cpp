#include "SparkGlobalHotkey.h"

#include <DApplication>
#include <QDebug>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <QtX11Extras/QX11Info>

DWIDGET_USE_NAMESPACE

SparkGlobalHotkey::SparkGlobalHotkey(QObject *parent) : QObject(parent), keyCode(0), modifiers(0) {
    //qApp->installNativeEventFilter(this); // 安装事件过滤器
    DApplication::instance()->installNativeEventFilter(this);
}

SparkGlobalHotkey::~SparkGlobalHotkey() {
    unregisterHotkey();
}

bool SparkGlobalHotkey::registerHotkey(const QKeySequence &keySequence) {
    if (keySequence.isEmpty()) return false;

    Display *display = QX11Info::display();
    if (!display) {
        qDebug() << "无法获取 X11 Display";
        return false;
    }

    Window root = QX11Info::appRootWindow();
    QString keyStr = keySequence.toString().split("+").last();
    KeySym keySym = XStringToKeysym(keyStr.toLatin1().constData());
    if (keySym == NoSymbol) {
        qDebug() << "无效的键:" << keyStr;
        return false;
    }

    keyCode = XKeysymToKeycode(display, keySym);
    modifiers = 0;
    if (keySequence[0] & Qt::ControlModifier) modifiers |= ControlMask;
    if (keySequence[0] & Qt::AltModifier) modifiers |= Mod1Mask;
    if (keySequence[0] & Qt::ShiftModifier) modifiers |= ShiftMask;

    qDebug() << "KeySym:" << keySym << "KeyCode:" << keyCode << "Modifiers:" << modifiers;

    XGrabKey(display, keyCode, modifiers, root, True, GrabModeAsync, GrabModeAsync);
    XSync(display, False);

    qWarning() << "全局快捷键已注册:" << keySequence.toString();
    return true;
}

void SparkGlobalHotkey::unregisterHotkey() {
    if (keyCode == 0) return;

    Display *display = QX11Info::display();
    if (display) {
        Window root = QX11Info::appRootWindow();
        XUngrabKey(display, keyCode, modifiers, root);
        XSync(display, False);
        qWarning() << "全局快捷键已注销";
    }
}

bool SparkGlobalHotkey::nativeEventFilter(const QByteArray &eventType, void *message, long *) {
    if (eventType != "x11") return false;

    XEvent *event = static_cast<XEvent *>(message);
    if (event->type == KeyPress) {
        XKeyEvent *keyEvent = &event->xkey;
        qDebug() << "KeyPress - KeyCode:" << keyEvent->keycode << "State:" << keyEvent->state;
        if (keyEvent->keycode == keyCode && (keyEvent->state & (ControlMask | Mod1Mask | ShiftMask)) == modifiers) {
            qWarning() << "快捷键触发!";
            emit activated();
            return true;
        }
    }
    return false;
}
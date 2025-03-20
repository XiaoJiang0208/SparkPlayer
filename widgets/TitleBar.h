#ifndef TITLE_BAR_H
#define TITLE_BAR_H

#include "../include.h"

DWIDGET_USE_NAMESPACE

class MouseHitBox : public DLabel
{
    Q_OBJECT  // 添加此行
private:
    /* data */
public:
    MouseHitBox(QWidget *parent):DLabel(parent){};

    void enterEvent(QEvent *event){
        emit onMouseEnter();
        DLabel::enterEvent(event);
    }
    void leaveEvent(QEvent *event){
        emit onMouseLeave();
        DLabel::leaveEvent(event);
    }

Q_SIGNALS:
    void onMouseEnter();
    void onMouseLeave();
};


class TitleBar : public DTitlebar
{
private:

public:
    TitleBar(QWidget *parent = nullptr);
    ~TitleBar();

    void setHide(bool on);
    void raise();

    //void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
};



#endif
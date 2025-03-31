#ifndef TITLE_BAR_H
#define TITLE_BAR_H

#include "../include.h"
#include "../utils/ImageTools.h"

DWIDGET_USE_NAMESPACE

class TitleBar : public DTitlebar
{
private:
    QWidget *background;
public:
    TitleBar(QWidget *parent = nullptr);
    ~TitleBar();



    void setHide(bool on);
    void raise();
    void setStyleSheet(const QString &styleSheet);

    void resizeEvent(QResizeEvent *event);
};



#endif
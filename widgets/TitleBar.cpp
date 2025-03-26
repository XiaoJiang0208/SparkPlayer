#include "TitleBar.h"

TitleBar::TitleBar(QWidget *parent):DTitlebar(parent)
{
    background = new QWidget(parent);
    this->setFixedHeight(50);
    this->setBackgroundTransparent(true);
    this->setIcon(this->style()->standardIcon(DStyle::SP_ComputerIcon));
}
 
TitleBar::~TitleBar(){
    
}

void TitleBar::setHide(bool on)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(300);
    QRect gm = this->geometry();
    animation->setStartValue(gm);
    if (on) {
        gm.setY(-50);
        animation->setEndValue(gm);
    } else {
        gm.setY(0);
        animation->setEndValue(gm);
    }
    animation->start(QAbstractAnimation::DeleteWhenStopped);

}

void TitleBar::raise()
{
    background->raise();
    DTitlebar::raise();
}

void TitleBar::setStyleSheet(const QString &styleSheet)
{
    background->setStyleSheet(styleSheet);
}

void TitleBar::resizeEvent(QResizeEvent *event)
{
    background->resize(this->size());
}

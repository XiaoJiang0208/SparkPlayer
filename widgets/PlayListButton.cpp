#include "PlayListButton.h"

PlayListButton::PlayListButton(QWidget *parent) : QPushButton(parent)
{
    is_show=false;
    initUI();
    connect(background, &QPushButton::clicked, this, &PlayListButton::hidePlayList);

}

PlayListButton::~PlayListButton()
{
}

void PlayListButton::initUI()
{
    this->setFixedSize(30,30);

    move(parentWidget()->width()-width()-10,35);
    this->setCheckable(false);
    this->setStyleSheet(".PlayListButton { background-color: rgba(196, 189, 189, 0); border-radius: 10px; }\
                         .PlayListButton:hover { background-color: rgba(196, 189, 189, 0.2); border-radius: 10px; }");
    this->setIcon(this->style()->standardIcon(DStyle::SP_DirOpenIcon));

    background = new QPushButton(window());
    background->setStyleSheet(".QPushButton { background-color: rgba(0,0,0,0) }");
    background->setFixedSize(window()->size());
    background->move(0,0);
    background->hide();
    play_list_box = new PlayListBox(window());
    play_list_box->setFixedSize(240,window()->size().height()-55-85);
    play_list_box->move(window()->size().width()+40,55);
}

void PlayListButton::showPlayList()
{
    is_show = true;
    background->show();
    background->raise();
    play_list_box->raise();
    QPropertyAnimation *animation = new QPropertyAnimation(play_list_box,"geometry");
    animation->setStartValue(play_list_box->geometry());
    animation->setEndValue(QRect(window()->size().width()-240,55,play_list_box->width(),play_list_box->width()));
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void PlayListButton::hidePlayList()
{
    is_show = false;
    background->hide();
    QPropertyAnimation *animation = new QPropertyAnimation(play_list_box,"geometry");
    animation->setStartValue(play_list_box->geometry());
    animation->setEndValue(QRect(window()->size().width()+40,55,play_list_box->width(),play_list_box->width()));
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void PlayListButton::reloadPlayList()
{
    
}

void PlayListButton::resizeEvent(QResizeEvent *event)
{
    move(parentWidget()->width()-width()-10,35);

    background->setFixedSize(window()->size());

    play_list_box->setFixedSize(240,window()->size().height()-55-85);
    if (is_show) {
        play_list_box->move(window()->size().width()-240,55);
    } else {
        play_list_box->move(window()->size().width()+40,55);
    }
    
}

void PlayListButton::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton) {
        showPlayList();
    }
}

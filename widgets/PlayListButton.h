#ifndef PLAYLISTBUTTON_H
#define PLAYLISTBUTTON_H

#include "../include.h"
#include "PlayListBox.h"

DWIDGET_USE_NAMESPACE

class PlayListButton  : public QPushButton
{
    Q_OBJECT;
private:
    QPushButton *background;

    PlayListBox *play_list_box;

    bool is_show;
public:
    PlayListButton(QWidget *parent = nullptr);
    ~PlayListButton();

    void initUI();

    // 显示播放列表
    void showPlayList();
    // 隐藏播放列表
    void hidePlayList();

    void reloadPlayList();

    void resizeEvent(QResizeEvent *event);
    void mouseReleaseEvent(QMouseEvent *e);

public Q_SLOTS:
    void slotThemeTypeChanged();
};


#endif // PLAYLISTBUTTON_H
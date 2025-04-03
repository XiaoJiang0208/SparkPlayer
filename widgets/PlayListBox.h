#ifndef PLAYLISTBOX_H
#define PLAYLISTBOX_H

#include "../include.h"
#include "../SparkMediaControler.h"
#include "MediaBox.h"

DWIDGET_USE_NAMESPACE

class PlayListBox : public QLabel
{
    Q_OBJECT
private:
    DScrollArea *media_list;
    DWidget *media_list_context;
    QVBoxLayout *media_list_context_layout;
    QLabel *title;

    std::deque<MediaBox *> media_buttons;
public:
    PlayListBox(QWidget *parent = nullptr);
    ~PlayListBox();

    void initUI();


public Q_SLOTS:
    void slotReloadPlayList();
    void slotThemeTypeChanged();
};



#endif // PLAYLISTBOX_H
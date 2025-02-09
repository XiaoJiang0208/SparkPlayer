#ifndef SPARKPLAYER_H
#define SPARKPLAYER_H

#include <DMainWindow>
#include <DTitlebar>
#include <DLabel>
#include <DFontSizeManager>
#include <DWidget>
#include <DPushButton>
#include <DButtonBox>
#include <DScrollArea>
#include <DSlider>
#include <DArrowButton>
#include <DStyle>
#include <QLayout>
#include <QResizeEvent>
#include "widgets/TimeLine.h"

DWIDGET_USE_NAMESPACE

class Sparkplayer : public DMainWindow
{
    Q_OBJECT

public:
    Sparkplayer();
    ~Sparkplayer();

    void setupUI();
protected:
    void resizeEvent(QResizeEvent *event);
private:
    QLabel *title;

    //QWidget *parent

    DWidget *main_box; // 右侧主窗口
    DScrollArea *media_list; // 左侧滚动列表
    DWidget *media_list_context; // 左侧媒体列表容器
    DWidget *controlers; // 下半部分控制器容器
    DTitlebar *title_bar; // 标题栏
    TimeLine *time_line;  // 时间线
    DPushButton *play_button; // 播放按钮
    DPushButton *previous_play; // 上一个
    DPushButton *next_play; // 下一个

    QList<DWidget*> media_list_items; // 媒体列表项

public slots:
    void slotThemeTypeChanged();
};
#endif // SPARKPLAYER_H

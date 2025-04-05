#ifndef SPARKPLAYER_H
#define SPARKPLAYER_H

#include "include.h"
#include "utils/Codec.h"
#include "widgets/TimeLine.h"
#include "widgets/MainPage.h"
#include "widgets/TitleBar.h"
#include "widgets/MediaListButton.h"
#include "widgets/VideoBox.h"
#include "widgets/VolumeBox.h"
#include "widgets/PlayListButton.h"
#include "SparkMediaControler.h"
#include "SparkSettings.h"

DWIDGET_USE_NAMESPACE

class Sparkplayer : public DMainWindow
{
    Q_OBJECT

public:
    Sparkplayer();
    ~Sparkplayer();

    TitleBar *getTitleBar(){return title_bar;};

protected:
    void resizeEvent(QResizeEvent *event);
private:
    QLabel *title;

    
    MainPage *main_page; // 主页面
    QList<PageData> page_data; // 页面数据
    static SparkMediaControler *media_controler;

    void setupUI();
    void reloadMediaPage();
    void addMediaPage(PageData data, int index = 0); // 添加媒体页面
    void removeMediaPage(MediaListButton *b);
    void setMainPage(QAbstractButton *button); // 设置主页面
    
    VideoBox *video_box;
    bool is_fullscreen = false;

    void hideControlers(bool t);

    //UI
    DWidget *main_box; // 右侧主窗口
    QVBoxLayout *main_box_layout; // 右侧主窗口布局器
    DScrollArea *media_list; // 左侧滚动列表
    DWidget *media_list_context; // 左侧媒体列表容器
    QVBoxLayout *media_list_context_layout; // 媒体列表纵向布局器
    QButtonGroup *media_list_buttons; // 媒体列表按钮组
    DWidget *bottom;
    DWidget *controlers; // 下半部分控制器容器
    TitleBar *title_bar; // 标题栏
    TimeLine *time_line;  // 时间线
    DPushButton *play_button; // 播放按钮
    DPushButton *previous_play; // 上一个
    DPushButton *next_play; // 下一个
    DPushButton *fullscreen_button; // 全屏按钮
    VolumeBox *volume_box;
    PlayListButton *play_list_button;
    DPushButton *test;
    

    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void slotThemeTypeChanged();
    void slotFullscreen(bool t);
    
};
#endif // SPARKPLAYER_H

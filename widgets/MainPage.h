#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <filesystem>

#include "../include.h"
//#include "ImageBox.h"
#include "../utils/ImageTools.h"
#include "../utils/Path.h"
#include "MediaBox.h"
#include "../SparkMediaControler.h"

namespace fs = std::filesystem;

DWIDGET_USE_NAMESPACE


enum PageType
{
    Box,
    List
};

struct PageData
{
    QString title = "MediaPage";
    QString content = "Content";
    QString path = "";
    QString time = "";
    PageType list_type = Box;
    

};

class MainPage : public DWidget
{
    Q_OBJECT
public:
    MainPage(QWidget *parent = nullptr, PageData *data = nullptr);
    
private:

    PageData *data; //页面数据
    
    QList<fs::path> media_data_list;
    QList<QWidget *> media_box_list; //媒体列表

    DWidget *media_list_bar; //主要标题栏
    QHBoxLayout *media_list_bar_layout; //主要标题栏布局器
    QVBoxLayout *media_list_bar_right_layout;
    QHBoxLayout *media_list_bar_button_layout; // 标题栏按钮布局
    DLabel *media_list_bar_title; //主要标题
    DLabel *media_list_bar_time; //主要时间
    DLabel *media_list_bar_content; //主要信息
    DLabel *media_list_bar_icon; //主要图标
    DScrollArea *media_list; //媒体列表
    DWidget *media_list_context; //媒体列表容器
    QVBoxLayout *media_list_context_layout; //媒体列表布局器

    DPushButton * play_all;

    void initUI(); //初始化页面
    void reloadMedia();

    void enableDragDrop(); //启用拖拽

    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);

public slots:
    void slotThemeTypeChanged();

signals:

};


#endif // MAINPAGE_H
#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <DSlider>
#include <DWidget>
#include <DLabel>
#include <DScrollArea>
#include <DPushButton>
#include <QHBoxLayout>
#include <QList>
#include <QUrl>

DWIDGET_USE_NAMESPACE


enum PageType
{
    Video,
    Audio
};

struct PageData
{
    QString title;
    QString content;
    QUrl url;
    QString time;
    PageType type;

};

class MainPage : public DWidget
{
    Q_OBJECT
public:
    explicit MainPage(QWidget *parent = nullptr);
    
private:
    void initUI(); //初始化页面
    DWidget *media_list_bar; //主要标题栏
    QVBoxLayout *media_list_bar_layout; //主要标题栏布局器
    DLabel *media_list_bar_title; //主要标题
    DScrollArea *media_list; //媒体列表
    DWidget *media_list_context; //媒体列表容器
    QVBoxLayout *media_list_context_layout; //媒体列表布局器

public slots:
    void slotThemeTypeChanged();

signals:

};


#endif // MAINPAGE_H
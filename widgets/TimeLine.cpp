#include "TimeLine.h"

TimeLine::TimeLine(Qt::Orientation orientation, QWidget *parent)
    : DWidget(parent)
{
    // 初始化控件
    main_slider = new QSlider(orientation,this);
    main_slider->setFixedHeight(20);
    //main_slider->setIconSize(QSize(5,5)); // 设置滑块大小
    left_time = new DLabel("00:00:00",this);
    right_time = new DLabel("00:00:00",this);
    
    // 暴露signal
    

    // 初始化布局器并布局
    QHBoxLayout * main_box_layout = new QHBoxLayout(this);
    main_box_layout->setContentsMargins(5,0,5,0);
    main_box_layout->addWidget(left_time);
    main_box_layout->addWidget(main_slider);
    main_box_layout->addWidget(right_time);

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &TimeLine::slotThemeTypeChanged);
    slotThemeTypeChanged();
}

void TimeLine::slotThemeTypeChanged(){
    main_slider->setStyleSheet("\
    .QSlider::groove:horizontal {\
        border: 0px solid #bbb;\
    }\
    .QSlider::sub-page:horizontal {\
        background: rgb(100, 180, 255);\
        border-radius: 2px;\
        margin-top:8px;\
        margin-bottom:8px;\
    }\
    .QSlider::add-page:horizontal {\
        background: rgb(231, 231, 231);\
        border-radius: 2px;\
        margin-top:8px;\
        margin-bottom:8px;\
    }\
    .QSlider::handle:horizontal {\
        background: rgb(193,204,208);\
        width: 5px;\
        border: 1px solid rgb(193,204,208);\
        border-radius: 2px; \
        margin-top:6px;\
        margin-bottom:6px;\
    }\
    .QSlider::handle:horizontal:hover {\
        background: rgb(193,204,208);\
        width: 10px;\
        border: 1px solid rgb(193,204,208);\
        border-radius: 5px; \
        margin-top:4px;\
        margin-bottom:4px;\
    }\
    ");
}
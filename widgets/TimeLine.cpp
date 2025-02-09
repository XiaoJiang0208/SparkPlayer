#include "TimeLine.h"

TimeLine::TimeLine(Qt::Orientation orientation, QWidget *parent)
    : DWidget(parent)
{
    // 初始化控件
    main_slider = new DSlider(orientation,this);
    main_slider->setIconSize(QSize(5,5)); // 设置滑块大小
    left_time = new DLabel("00:00:00",this);
    right_time = new DLabel("00:00:00",this);
    
    // 暴露signal
    

    // 初始化布局器并布局
    QHBoxLayout * main_box_layout = new QHBoxLayout(this);
    main_box_layout->setContentsMargins(5,0,5,0);
    main_box_layout->addWidget(left_time);
    main_box_layout->addWidget(main_slider);
    main_box_layout->addWidget(right_time);
}

void TimeLine::slotThemeTypeChanged(){
    
}
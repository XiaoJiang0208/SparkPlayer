#ifndef TIMELINE_H
#define TIMELINE_H

#include "../include.h"

DWIDGET_USE_NAMESPACE

class TimeLine : public DWidget
{
    Q_OBJECT
public:
    explicit TimeLine(Qt::Orientation orientation = Qt::Horizontal, QWidget *parent = nullptr);
    QSlider *main_slider;
    
    void setMaxTime(double time);
    void setTime(double time);

private:
    DLabel * left_time;
    DLabel * right_time;

    double max_time;
public slots:
    void slotThemeTypeChanged();
signals:

};

#endif // TIMELINE_H

#ifndef TIMELINE_H
#define TIMELINE_H

#include <DSlider>
#include <DWidget>
#include <DLabel>
#include <QHBoxLayout>
#include <QList>

DWIDGET_USE_NAMESPACE

class TimeLine : public DWidget
{
    Q_OBJECT
public:
    explicit TimeLine(Qt::Orientation orientation = Qt::Horizontal, QWidget *parent = nullptr);
    DSlider *main_slider;
    
private:
    DLabel * left_time;
    DLabel * right_time;

public slots:
    void slotThemeTypeChanged();

signals:

};

#endif // TIMELINE_H

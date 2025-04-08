#ifndef SPEEDBOX_H
#define SPEEDBOX_H

#include "../include.h"
#include "../SparkMediaControler.h"

DWIDGET_USE_NAMESPACE

class SpeedSettingBox : public QLabel
{
    Q_OBJECT
private:
    int volume;
    QPushButton *speed0_25;
    QPushButton *speed0_5;
    QPushButton *speed0_75;
    QPushButton *speed1_0;
    QPushButton *speed1_25;
    QPushButton *speed1_5;
    QPushButton *speed2_0;
public:
    SpeedSettingBox(QWidget *parent = nullptr);
    ~SpeedSettingBox();

    //void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

Q_SIGNALS:
    void onSetVolume(double value);

public Q_SLOTS:
    void slotThemeTypeChanged();

};


class SpeedBox : public QPushButton
{
    Q_OBJECT
private:
    QLabel *icon;
    SpeedSettingBox *speed_setting_box;
public:
    SpeedBox(QWidget *parent = nullptr);
    ~SpeedBox();

    void initUI();

    void resizebox();
    void raise();

    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

};



#endif // SPEEDBOX_H
#ifndef VOLUMEBOX_H
#define VOLUMEBOX_H

#include "../include.h"
#include "../SparkMediaControler.h"

DWIDGET_USE_NAMESPACE

class VolumeSettingBox : public QLabel
{
    Q_OBJECT
private:
    int volume;
    QSlider *volume_slider;

public:
    VolumeSettingBox(QWidget *parent = nullptr);
    ~VolumeSettingBox();

    //void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

public Q_SLOTS:
    void slotSetVolume(int value);
    void slotThemeTypeChanged();

};


class VolumeBox : public QPushButton
{
    Q_OBJECT
private:
    QLabel *icon;
    VolumeSettingBox *volume_setting_box;
public:
    VolumeBox(QWidget *parent = nullptr);
    ~VolumeBox();

    void initUI();


    void resizebox();
    void raise();

    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

};



#endif // VOLUMEBOX_H
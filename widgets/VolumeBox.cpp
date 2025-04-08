#include "VolumeBox.h"

VolumeBox::VolumeBox(QWidget *parent):QPushButton(parent)
{
    
    initUI();
    
}

VolumeBox::~VolumeBox()
{
}

void VolumeBox::initUI()
{
    setFixedSize(30,30);
    this->setStyleSheet(".VolumeBox { background-color: rgba(196, 189, 189, 0); border-radius: 10px; }"
                        ".VolumeBox:hover { background-color: rgba(196, 189, 189, 0.2); border-radius: 10px; }"
                        ".VolumeBox:pressed { background-color: rgba(196, 189, 189, 0.3); border-radius: 10px; }");
    icon = new QLabel(this);
    icon->setFixedSize(this->size());
    icon->setPixmap(this->style()->standardIcon(DStyle::SP_MediaVolume).pixmap(this->size()));
    icon->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    move(parentWidget()->width()-width()-40,35);
    volume_setting_box = new VolumeSettingBox(window());
    volume_setting_box->move(this->x()-3,window()->height()-80+y()-80);
    volume_setting_box->hide();
}
 
void VolumeBox::resizebox()
{
    move(parentWidget()->width()-width()-40,35);
    volume_setting_box->move(this->x()-3,window()->height()-80+y()-80);
}

void VolumeBox::raise()
{
    volume_setting_box->raise();
    DPushButton::raise();
}

void VolumeBox::enterEvent(QEvent *event)
{
    volume_setting_box->show();
}

void VolumeBox::leaveEvent(QEvent *event)
{
    QTimer::singleShot(100, this, [this](){
        if (!volume_setting_box->underMouse())
        {
            volume_setting_box->hide();
        }
        
    });
}

VolumeSettingBox::VolumeSettingBox(QWidget *parent) : QLabel(parent)
{
    this->setFixedSize(36,80);
    this->setLayout(new QVBoxLayout());
    this->layout()->setContentsMargins(0,5,0,5);
    volume_slider = new QSlider(Qt::Orientation::Vertical,this);
    volume_slider->setTickInterval(100);
    volume_slider->setMaximum(100);
    volume_slider->setValue(100);
    this->layout()->addWidget(volume_slider);
    connect(volume_slider, &QAbstractSlider::valueChanged,
        this, &VolumeSettingBox::slotSetVolume);
    connect(volume_slider,&QSlider::valueChanged,this,&VolumeSettingBox::slotSetVolume);

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &VolumeSettingBox::slotThemeTypeChanged);
    slotThemeTypeChanged();
}

VolumeSettingBox::~VolumeSettingBox()
{
}

void VolumeSettingBox::leaveEvent(QEvent *event)
{
    hide();
}

void VolumeSettingBox::slotSetVolume(int value) {
    volume = value;
    SparkMediaControler::getInstance()->setVolume(volume);
}

void VolumeSettingBox::slotThemeTypeChanged(){
    this->setStyleSheet(".VolumeSettingBox { background-color: #ffffff; border-radius: 10px; }");
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(30);    // 模糊半径
    shadow->setOffset(0, 0 );        // 偏移量
    shadow->setColor(qRgb(51,51,51));    // 阴影颜色
    this->setGraphicsEffect(shadow);
    volume_slider->setStyleSheet("\
    .QSlider::groove:vertical {\
        border: 0px solid #bbb;\
    }\
    .QSlider::sub-page:vertical {\
        background: rgb(231, 231, 231);\
        border-radius: 2px;\
        margin-left:14px;\
        margin-right:14px;\
    }\
    .QSlider::add-page:vertical {\
        background: rgb(100, 180, 255);\
        border-radius: 2px;\
        margin-left:14px;\
        margin-right:14px;\
    }\
    .QSlider::handle:vertical {\
        background: rgb(193,204,208);\
        width: 5px;\
        height: 5px;\
        border: 1px solid rgb(193,204,208);\
        border-radius: 2px; \
        margin-left:12px;\
        margin-right:12px;\
    }\
    .QSlider::handle:vertical:hover {\
        background: rgb(193,204,208);\
        width: 10px;\
        height: 10px;\
        border: 1px solid rgb(193,204,208);\
        border-radius: 5px; \
        margin-left:10px;\
        margin-right:10px;\
    }\
    ");
}
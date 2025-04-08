#include "SpeedBox.h"

SpeedBox::SpeedBox(QWidget *parent):QPushButton(parent)
{
    
    initUI();
    
}

SpeedBox::~SpeedBox()
{
}

void SpeedBox::initUI()
{
    setFixedSize(30,30);
    this->setStyleSheet(".SpeedBox { background-color: rgba(196, 189, 189, 0); border-radius: 10px; }"
                        ".SpeedBox:hover { background-color: rgba(196, 189, 189, 0.2); border-radius: 10px; }"
                        ".SpeedBox:pressed { background-color: rgba(196, 189, 189, 0.3); border-radius: 10px; }");
    icon = new QLabel(this);
    icon->setFixedSize(this->size());
    // dtk倍速图标
    icon->setText("1.0x");
    //icon->setPixmap(this->style()->standardIcon(DStyle::SP_).pixmap(this->size()));
    icon->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    move(parentWidget()->width()-width()-70,35);
    speed_setting_box = new SpeedSettingBox(window());
    connect(speed_setting_box,&SpeedSettingBox::onSetVolume,[=](double value){
        icon->setText(QString::number(value, 'f', 2));
    });
    speed_setting_box->move(this->x()-8,window()->height()-80+y()-80);
    speed_setting_box->hide();
}

void SpeedBox::resizebox()
{
    speed_setting_box->move(this->x()-8,window()->height()-80+y()-80);
    move(parentWidget()->width()-width()-70,35);
}

void SpeedBox::raise()
{
    speed_setting_box->raise();
    DPushButton::raise();
}

void SpeedBox::enterEvent(QEvent *event)
{
    speed_setting_box->raise();
    speed_setting_box->show();
}

void SpeedBox::leaveEvent(QEvent *event)
{
    QTimer::singleShot(100, this, [this](){
        if (!speed_setting_box->underMouse())
        {
            speed_setting_box->hide();
        }
        
    });
}

SpeedSettingBox::SpeedSettingBox(QWidget *parent) : QLabel(parent)
{
    this->setFixedSize(46,120);
    this->setLayout(new QVBoxLayout());
    this->layout()->setContentsMargins(0,5,0,5);
    this->layout()->setSpacing(0);
    speed0_25 = new QPushButton("0.25x",this);
    connect(speed0_25,&QPushButton::clicked,[=](){
        if (!SparkMediaControler::getInstance()->isHaveFile()) return;
        SparkMediaControler::getInstance()->setPlaybackSpeed(0.25);
        emit onSetVolume(0.25);
    });
    speed0_5 = new QPushButton("0.5x",this);
    connect(speed0_5,&QPushButton::clicked,[=](){
        if (!SparkMediaControler::getInstance()->isHaveFile()) return;
        SparkMediaControler::getInstance()->setPlaybackSpeed(0.5);
        emit onSetVolume(0.5);
    });
    speed0_75 = new QPushButton("0.75x",this);
    connect(speed0_75,&QPushButton::clicked,[=](){
        if (!SparkMediaControler::getInstance()->isHaveFile()) return;
        SparkMediaControler::getInstance()->setPlaybackSpeed(0.75);
        emit onSetVolume(0.75);
    });
    speed1_0 = new QPushButton("1.0x",this);
    connect(speed1_0,&QPushButton::clicked,[=](){
        if (!SparkMediaControler::getInstance()->isHaveFile()) return;
        SparkMediaControler::getInstance()->setPlaybackSpeed(1.0);
        emit onSetVolume(1.0);
    });
    speed1_25 = new QPushButton("1.25x",this);
    connect(speed1_25,&QPushButton::clicked,[=](){
        if (!SparkMediaControler::getInstance()->isHaveFile()) return;
        SparkMediaControler::getInstance()->setPlaybackSpeed(1.25);
        emit onSetVolume(1.25);
    });
    speed1_5 = new QPushButton("1.5x",this);
    connect(speed1_5,&QPushButton::clicked,[=](){
        if (!SparkMediaControler::getInstance()->isHaveFile()) return;
        SparkMediaControler::getInstance()->setPlaybackSpeed(1.5);
        emit onSetVolume(1.5);
    });
    speed2_0 = new QPushButton("2.0x",this);
    connect(speed2_0,&QPushButton::clicked,[=](){
        if (!SparkMediaControler::getInstance()->isHaveFile()) return;
        SparkMediaControler::getInstance()->setPlaybackSpeed(2.0);
        emit onSetVolume(2.0);
    });
    this->layout()->addWidget(speed0_25);
    this->layout()->addWidget(speed0_5);
    this->layout()->addWidget(speed0_75);
    this->layout()->addWidget(speed1_0);
    this->layout()->addWidget(speed1_25);
    this->layout()->addWidget(speed1_5);
    this->layout()->addWidget(speed2_0);
    slotThemeTypeChanged();
}

SpeedSettingBox::~SpeedSettingBox()
{
}

void SpeedSettingBox::leaveEvent(QEvent *event)
{
    hide();
}

void SpeedSettingBox::slotThemeTypeChanged(){
    this->setStyleSheet(".SpeedSettingBox { background-color: #ffffff; border-radius: 10px; }");
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(30);    // 模糊半径
    shadow->setOffset(0, 0 );        // 偏移量
    shadow->setColor(qRgb(51,51,51));    // 阴影颜色
    this->setGraphicsEffect(shadow);
    speed0_25->setStyleSheet(".QPushButton{background-color:rgba(196, 189, 189, 0); border-radius: 20px;}\
                                .QPushButton:hover{background-color:rgba(196, 189, 189, 0.2); border-radius: 20px;}\
                                .QPushButton:pressed{background-color:rgba(196, 189, 189, 0.3); border-radius: 20px;}");
    speed0_5->setStyleSheet(speed0_25->styleSheet());
    speed0_75->setStyleSheet(speed0_25->styleSheet());
    speed1_0->setStyleSheet(speed0_25->styleSheet());
    speed1_25->setStyleSheet(speed0_25->styleSheet());
    speed1_5->setStyleSheet(speed0_25->styleSheet());
    speed2_0->setStyleSheet(speed0_25->styleSheet());
}
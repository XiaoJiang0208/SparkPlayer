#include "sparkplayer.h"
#include "./ui_sparkplayer.h"

Sparkplayer::Sparkplayer()
    : DMainWindow()
{
    setupUI();
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &Sparkplayer::slotThemeTypeChanged);
    slotThemeTypeChanged();
}

Sparkplayer::~Sparkplayer()
{
}

void Sparkplayer::setupUI()
{
    //ui->setupUi(this); // .ui文件在转换为头文件时无法引用Dtk的头文件
    //titlebar()->setTitle("SparkPlayer");
    //titlebar()->setAutoHideOnFullscreen(true);
    //titlebar()->setBackgroundTransparent(true);
    titlebar()->deleteLater();
    title_bar = new DTitlebar(this);
    title_bar->setFixedHeight(50);
    title_bar->setBackgroundTransparent(true);
    title_bar->setIcon(title_bar->style()->standardIcon(DStyle::SP_ComputerIcon));
    setTitlebarShadowEnabled(false); // 取消标题栏阴影
    setWindowTitle("SparkPlayer"); // 设置标题栏的标题
    setMinimumSize(480,270); // 长宽比使用16:9适应大部分视屏比例
    resize(960,540);
    // 得手动整一个centralWidget有点无语
    DWidget *centralwidget = new DWidget(this); // 中央容器
    //centralwidget->setStyleSheet(".QWidget{background-color: black;}");
    QVBoxLayout *centralwidget_layout = new QVBoxLayout(centralwidget); // 中央容器布局器
    centralwidget_layout->setMargin(0);
    centralwidget_layout->setSpacing(0);
    layout()->setMargin(0);
    setCentralWidget(centralwidget);
    titlebar()->setDisableFlags(Qt::WindowMinimizeButtonHint);


    // 上半部分布局器
    QHBoxLayout *up_layout = new QHBoxLayout();
    QVBoxLayout *left_layout = new QVBoxLayout(); // 左侧布局器
    up_layout->addItem(left_layout);
    //标题
    title = new QLabel("SparkPlayer",this);
    title->setContentsMargins(50,0,0,0);
    title->setFixedHeight(50);
    left_layout->addWidget(title);
    // 左侧媒体列表
    media_list = new DScrollArea(centralWidget()); // 初始化滚动列表
    left_layout->addWidget(media_list);
    media_list->setFixedWidth(200);
    media_list_context = new DWidget(); // 初始化左侧媒体列表容器
    QVBoxLayout *media_list_layout = new QVBoxLayout(media_list_context); // 媒体列表纵向布局器
    media_list_layout->setContentsMargins(10,10,0,0);
    for (int i = 0; i < 10; i++)
    {
        DPushButton *b = new DPushButton(QString("button%1").arg(i),media_list_context);
        b->setFixedHeight(50);
        b->setFixedWidth(180);
        b->setIcon(b->style()->standardIcon(DStyle::SP_ComputerIcon));
        media_list_items.append(b);
        media_list_layout->addWidget(b);
    }
    //QSpacerItem *s =new  QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding); // 弹簧
    media_list_layout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding)); // 弹簧
    media_list->setWidget(media_list_context);
    

    main_box = new DWidget(centralWidget()); // 右侧主窗口
    up_layout->addWidget(main_box); 
    centralwidget_layout->addLayout(up_layout); // 添加到上半部分的布局器中
    DPushButton *t = new DPushButton("test button",main_box);


    // 下半控制部分
    controlers = new DWidget(centralWidget()); // 控制部分容器
    controlers->setFixedHeight(80);
    QVBoxLayout *controlers_layout = new QVBoxLayout(controlers); // 控制器竖列布局器
    controlers_layout->setContentsMargins(0,0,0,20);
    controlers_layout->setSpacing(0);
    centralwidget_layout->addWidget(controlers);

    time_line = new TimeLine(Qt::Horizontal,controlers); // 初始化时间线
    controlers_layout->addWidget(time_line);

    QHBoxLayout *controler_box_layout = new QHBoxLayout(); // 控制器横排布局器
    controler_box_layout->setSpacing(10);
    controlers_layout->addItem(controler_box_layout);

    play_button = new DPushButton(controlers); // 初始化播放按钮
    play_button->setIcon(play_button->style()->standardIcon(DStyle::SP_MediaPlay));
    play_button->setIconSize(QSize(25,25));
    play_button->setFixedSize(40,40);
    play_button->setStyleSheet(".QPushButton{background-color:rgb(100, 180, 255); border-radius: 20px;}\
                                .QPushButton:hover{background-color:rgb(74, 143, 207); border-radius: 20px;}\
                                .QPushButton:pressed{background-color:rgb(117, 163, 206); border-radius: 20px;}");
    previous_play = new DPushButton(controlers); // 初始化上一个按钮
    previous_play->setIcon(previous_play->style()->standardIcon(DStyle::SP_MediaSeekBackward));
    previous_play->setIconSize(QSize(40,40));
    previous_play->setFixedSize(40,40);
    previous_play->setStyleSheet(".QPushButton{background-color:rgba(196, 189, 189, 0); border-radius: 20px;}\
                                .QPushButton:hover{background-color:rgba(196, 189, 189, 0.2); border-radius: 20px;}\
                                .QPushButton:pressed{background-color:rgba(196, 189, 189, 0.53); border-radius: 20px;}");
    next_play = new DPushButton(controlers); // 初始化下一个按钮
    next_play->setIcon(next_play->style()->standardIcon(DStyle::SP_MediaSeekForward));
    next_play->setIconSize(QSize(40,40));
    next_play->setFixedSize(40,40);
    next_play->setStyleSheet(".QPushButton{background-color:rgba(196, 189, 189, 0); border-radius: 20px;}\
                                .QPushButton:hover{background-color:rgba(196, 189, 189, 0.2); border-radius: 20px;}\
                                .QPushButton:pressed{background-color:rgba(196, 189, 189, 0.5); border-radius: 20px;}");
    
    controler_box_layout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum)); // 弹簧
    controler_box_layout->addWidget(previous_play);
    controler_box_layout->addWidget(play_button);
    controler_box_layout->addWidget(next_play);
    controler_box_layout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum)); // 弹簧

    title_bar->raise(); // 置顶 titlebar

}

void Sparkplayer::resizeEvent(QResizeEvent *event)
{
    DMainWindow::resizeEvent(event);
    title_bar->setFixedWidth(event->size().width());
}

void Sparkplayer::slotThemeTypeChanged(){
    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType)
    {
        title->setStyleSheet(".QLabel{background-color: #f0f3f6;}");
        media_list_context->setStyleSheet(".QWidget{background-color: #f0f3f6;}");
        media_list->setStyleSheet(".QScrollArea{background-color: #f0f3f6;border-style: none;}");
        main_box->setStyleSheet(".QWidget{background-color: #f7f9fc;}");
        controlers->setStyleSheet(".QWidget{background-color: #fafafa;}");
        for(auto item : media_list_items)
        {
            item->setStyleSheet(".QPushButton{background-color:rgba(240, 243, 246, 0); border-radius: 10px;}\
                  .QPushButton:hover{background-color: #e1e4e8; border-radius: 10px;}\
                  .QPushButton:pressed{background-color: #d2d5d9; border-radius: 10px;}");
        }


    } else {
        title->setStyleSheet(".QLabel{background-color: #1a1a21;}");
        media_list_context->setStyleSheet(".QWidget{background-color: #1a1a21;}");
        media_list->setStyleSheet(".QScrollArea{background-color: #1a1a21;border-style: none;}");
        main_box->setStyleSheet(".QWidget{background-color: #13131a;}");
        controlers->setStyleSheet(".QWidget{background-color: #2d2d38;}");
        for (auto item : media_list_items)
        {
            item->setStyleSheet(".QPushButton{background-color:rgba(26, 26, 33, 0); border-radius: 10px;}\
                  .QPushButton:hover{background-color: #1a1a21; border-radius: 10px;}\
                  .QPushButton:pressed{background-color: #0d0d0f; border-radius: 10px;}");
        }
    }
    
}
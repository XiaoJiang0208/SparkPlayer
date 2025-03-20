#include "sparkplayer.h"
//#include "./ui_sparkplayer.h"

Sparkplayer::Sparkplayer()
    : DMainWindow()
{
    main_page = nullptr;
    media_controler = new SparkMediaControler();
    media_controler->reSize(960,540);
    setupUI();
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &Sparkplayer::slotThemeTypeChanged);
    connect(media_controler,&SparkMediaControler::onImageDone,this,&Sparkplayer::showimg);
    slotThemeTypeChanged();
    media_controler->openMedia("/home/xiaojiang/Desktop/ttt/1.mp4");

    addMediaPage(PageData{"主页","shit","","",Box},-1);
    addMediaPage(PageData{"音乐","Content2","~/Music","",Box},-1);
    addMediaPage(PageData{"视屏","Content3","/home/xiaojiang/Desktop/ttt","",Box},-1);
}

Sparkplayer::~Sparkplayer()
{
    delete media_controler;
}

void Sparkplayer::setupUI()
{
    //ui->setupUi(this); // .ui文件在转换为头文件时无法引用Dtk的头文件
    //titlebar()->setTitle("SparkPlayer");
    //titlebar()->setAutoHideOnFullscreen(true);
    //titlebar()->setBackgroundTransparent(true);
    titlebar()->deleteLater();
    title_bar = new TitleBar(this);
    //title_bar->setTitle("ttt");
    setTitlebarShadowEnabled(false); // 取消标题栏阴影
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
    title = new QLabel(tr("SparkPlayer"),this);
    title->setAlignment(Qt::AlignCenter);
    title->setFixedSize(200,50);
    left_layout->addWidget(title);


    // 左侧媒体列表
    media_list = new DScrollArea(centralWidget()); // 初始化滚动列表
    left_layout->addWidget(media_list);
    media_list->setFixedWidth(200);
    media_list_context = new DWidget(); // 初始化左侧媒体列表容器
    media_list->setWidgetResizable(true);
    media_list_context->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 横向铺满
    media_list_context_layout = new QVBoxLayout(media_list_context); // 媒体列表纵向布局器
    media_list_context_layout->setContentsMargins(10,10,0,0);
    media_list_context_layout->setAlignment(Qt::AlignTop); // 从上到下排列
    media_list_buttons = new QButtonGroup(media_list_context); // 媒体列表按钮组
    connect(media_list_buttons, static_cast<void(QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked),
            this, &Sparkplayer::setMainPage);
    // DPushButton *b = new DPushButton(QString("addone"),media_list_context);
    // b->setFixedHeight(50);
    // b->setFixedWidth(180);
    // b->setCheckable(true);
    // b->setIcon(b->style()->standardIcon(DStyle::SP_ComputerIcon));
    // media_list_buttons->addButton(b);
    // media_list_context_layout->addWidget(b);

    //QSpacerItem *s =new  QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding); // 弹簧
    media_list->setWidget(media_list_context);
    


    // 右侧区域
    main_box = new DWidget(centralWidget()); // 右侧主窗口
    main_box_layout = new QVBoxLayout(main_box); // 主窗口纵向布局器
    main_box_layout->setContentsMargins(0,0,0,0);
    main_box_layout->setSpacing(0);
    main_box_layout->addItem(new QSpacerItem(20, 50, QSizePolicy::Minimum, QSizePolicy::Fixed)); // 弹簧
    up_layout->addWidget(main_box);
    centralwidget_layout->addLayout(up_layout); // 添加到上半部分的布局器中
    //setMainPage(PageData()); // 设置主页面


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
    controler_box_layout->setAlignment(Qt::AlignCenter);
    controlers_layout->addItem(controler_box_layout);

    play_button = new DPushButton(controlers); // 初始化播放按钮
    play_button->setIcon(play_button->style()->standardIcon(DStyle::SP_MediaPlay));
    play_button->setIconSize(QSize(25,25));
    play_button->setFixedSize(40,40);
    play_button->setStyleSheet(".QPushButton{background-color:rgb(100, 180, 255); border-radius: 20px;}\
                                .QPushButton:hover{background-color:rgb(74, 143, 207); border-radius: 20px;}\
                                .QPushButton:pressed{background-color:rgb(117, 163, 206); border-radius: 20px;}");
    connect(play_button,&DPushButton::clicked,[=](){
        // PageData data = PageData{"新页面","Content",QUrl(""),"",Box};
        // addMediaPage(data);
        if (!media_controler->getStatus())
        {
            media_controler->play();
            if (media_controler->getStatus()) 
                play_button->setIcon(play_button->style()->standardIcon(DStyle::SP_MediaPause));
        } else {
            media_controler->pause();
            if (!media_controler->getStatus()) 
                play_button->setIcon(play_button->style()->standardIcon(DStyle::SP_MediaPlay));
        }
        
    });
    connect(media_controler,&SparkMediaControler::onStatusChange,[=](){
        // PageData data = PageData{"新页面","Content",QUrl(""),"",Box};
        // addMediaPage(data);
        if (!media_controler->getStatus())
        {
            play_button->setIcon(play_button->style()->standardIcon(DStyle::SP_MediaPause));
        } else
        {
            play_button->setIcon(play_button->style()->standardIcon(DStyle::SP_MediaPlay));
        }
        
    });
    previous_play = new DPushButton(controlers); // 初始化上一个按钮
    previous_play->setIcon(previous_play->style()->standardIcon(DStyle::SP_MediaSeekBackward));
    previous_play->setIconSize(QSize(40,40));
    previous_play->setFixedSize(40,40);
    previous_play->setStyleSheet(".QPushButton{background-color:rgba(196, 189, 189, 0); border-radius: 20px;}\
                                .QPushButton:hover{background-color:rgba(196, 189, 189, 0.2); border-radius: 20px;}\
                                .QPushButton:pressed{background-color:rgba(196, 189, 189, 0.3); border-radius: 20px;}");
    connect(previous_play,&DPushButton::clicked,[=](){
        media_controler->setSeekTime(0.0);
    });
    next_play = new DPushButton(controlers); // 初始化下一个按钮
    next_play->setIcon(next_play->style()->standardIcon(DStyle::SP_MediaSeekForward));
    next_play->setIconSize(QSize(40,40));
    next_play->setFixedSize(40,40);
    next_play->setStyleSheet(".QPushButton{background-color:rgba(196, 189, 189, 0); border-radius: 20px;}\
                                .QPushButton:hover{background-color:rgba(196, 189, 189, 0.2); border-radius: 20px;}\
                                .QPushButton:pressed{background-color:rgba(196, 189, 189, 0.3); border-radius: 20px;}");
    connect(next_play,&DPushButton::clicked,[=](){
        title_bar->setHide(true);
    });
    
    //controler_box_layout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum)); // 弹簧
    controler_box_layout->addWidget(previous_play);
    controler_box_layout->addWidget(play_button);
    controler_box_layout->addWidget(next_play);
    //controler_box_layout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum)); // 弹簧

    test = new DLabel(this);
    test->move(100,0);
    test->setFixedSize(960,540);
    test->resize(960,540);
    title_bar->raise(); // 置顶 titlebar
}

void Sparkplayer::resizeEvent(QResizeEvent *event)
{
    DMainWindow::resizeEvent(event);
    title_bar->setFixedWidth(event->size().width());

}

void Sparkplayer::reloadMediaPage()
{
    int index = 0;
    for(QList<PageData>::iterator data = page_data.begin();data < page_data.end();data++){
        PageData *pdata = &(*data);
        int i =media_list_buttons->buttons().size();
        if (media_list_buttons->buttons().size()<=index) {
            MediaListButton *b = new MediaListButton(QString("%1").arg(pdata->title),media_list_context);
            b->setFixedHeight(50);
            b->setFixedWidth(180);
            b->setCheckable(true);
            b->setIcon(Path::applicationPath("images/bg.png").toString());
            b->setData(pdata);
            b->setStyleSheet(".QPushButton{background-color:rgba(196, 189, 189, 0); border-radius: 10px;}\
                                .QPushButton:hover{background-color:rgba(196, 189, 189, 0.2); border-radius: 10px;}\
                                .QPushButton:pressed{background-color:rgba(196, 189, 189, 0.3); border-radius: 10px;}\
                                .QPushButton:checked{background-color:rgb(100, 180, 255); border-radius: 10px;}");
            media_list_buttons->addButton(b);
            media_list_context_layout->insertWidget(index,b);
        }
        MediaListButton *pButton = (MediaListButton *)(media_list_buttons->buttons()[index]);
        PageData *d=pButton->getData();
        if (pdata != pButton->getData())
        {
            pButton->setData(pdata);
        }
        index++;
    }
    
}

void Sparkplayer::addMediaPage(PageData data, int index)
{
    if (index < 0) {   
        page_data.push_back(data);
    } else {
        QList<PageData>::iterator id = page_data.begin()+index;
        page_data.insert(id,data);

    }
    
    reloadMediaPage();
    qDebug() << "add page " << data.title;
}

void Sparkplayer::setMainPage(QAbstractButton *button)
{
    if (main_page)
    {
        //layout()->removeWidget(main_page);
        main_page->deleteLater();
        main_page = nullptr;
    }
    main_page = new MainPage(main_box,((MediaListButton *)button)->getData());
    if (main_page)
    {
        main_box_layout->addWidget(main_page);
        
    }
    
}

void Sparkplayer::showimg()
{
    test->setPixmap(QPixmap::fromImage(*(media_controler->getImg())));
}

void Sparkplayer::slotThemeTypeChanged(){

    for(auto item : media_list_buttons->buttons())
    {
        item->setStyleSheet(".QPushButton{background-color:rgba(196, 189, 189, 0); border-radius: 10px;}\
                            .QPushButton:hover{background-color:rgba(196, 189, 189, 0.2); border-radius: 10px;}\
                            .QPushButton:pressed{background-color:rgba(196, 189, 189, 0.3); border-radius: 10px;}\
                            .QPushButton:checked{background-color:rgb(100, 180, 255); border-radius: 10px;}");
    }
    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType)
    {
        title->setStyleSheet(".QLabel{background-color: #f0f3f6;}");
        media_list_context->setStyleSheet(".QWidget{background-color: #f0f3f6;}");
        media_list->setStyleSheet(".QScrollArea{background-color: #f0f3f6;border-style: none;}");
        main_box->setStyleSheet(".QWidget{background-color: #f7f9fc;}");
        controlers->setStyleSheet(".QWidget{background-color: #fafafa;}");


    } else {
        title->setStyleSheet(".QLabel{background-color: #1a1a21;}");
        media_list_context->setStyleSheet(".QWidget{background-color: #1a1a21;}");
        media_list->setStyleSheet(".QScrollArea{background-color: #1a1a21;border-style: none;}");
        main_box->setStyleSheet(".QWidget{background-color: #13131a;}");
        controlers->setStyleSheet(".QWidget{background-color: #2d2d38;}");

    }
    
}
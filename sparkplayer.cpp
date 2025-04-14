#include "sparkplayer.h"
//#include "./ui_sparkplayer.h"

SparkMediaControler* Sparkplayer::media_controler = nullptr;

Sparkplayer::Sparkplayer()
    : DMainWindow()
{
    setWindowIcon(QIcon(ImageTools::toPixmap(Path::applicationPath("images/icon.png").toString(),{1000,1000})));
    main_page = nullptr;
    media_controler = SparkMediaControler::getInstance();
    //media_controler->setVideoSize(0,0);
    setupUI();

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &Sparkplayer::slotThemeTypeChanged);
    //connect(media_controler,&SparkMediaControler::onImageDone,this,&Sparkplayer::showimg);
    connect(video_box,&VideoBox::onFullscreen,this,&Sparkplayer::slotFullscreen);
    connect(video_box,&VideoBox::onNeedShow,this,[&](){hideControlers(false);title_bar->setHide(false);});
    connect(video_box,&VideoBox::onNeedHide,this,[&](){hideControlers(true);title_bar->setHide(true);});
    connect(fullscreen_button,&DPushButton::clicked,[&](){video_box->fullscreen(false);});
    connect(ai_button,&DPushButton::clicked,this,&Sparkplayer::slotAI);
    connect(playmode_button,&DPushButton::clicked,this,&Sparkplayer::slotChangePlayMode);

    slotThemeTypeChanged();


    addMediaPage(PageData{"首页","","","",List},-1);
    addMediaPage(PageData{"音乐","Content2","/home/"+QString(std::getenv("USER"))+"/Music","",Box},-1);
    addMediaPage(PageData{"视频","Content3","/home/"+QString(std::getenv("USER"))+"/Videos","",Box},-1);
    for (auto i : SparkSettings::getInstance()->getMediaLibraries())
    {
        QString dir = i.c_str();
        if (!dir.isEmpty()) {
            QString title = QFileInfo(dir).fileName();
            addMediaPage(PageData{ title, "", dir, "", Box }, -1);
        }
    }

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
    title_bar = new TitleBar(this);
    //title_bar->setTitle("ttt");
    setTitlebarShadowEnabled(false); // 取消标题栏阴影
    setMinimumSize(600,338); // 长宽比使用16:9适应大部分视频比例
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


    // 下半控制部分------
    bottom = new DWidget(centralWidget());
    bottom->setFixedHeight(80);
    centralwidget_layout->addWidget(bottom);
    controlers = new DWidget(this); // 控制部分容器
    controlers->setFixedHeight(80);
    controlers->setFixedWidth(width());
    controlers->move(0,height()-80);
    QVBoxLayout *controlers_layout = new QVBoxLayout(controlers); // 控制器竖列布局器
    controlers_layout->setContentsMargins(0,0,0,20);
    controlers_layout->setSpacing(0);

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
        } else {
            media_controler->pause();
        }
        
    });
    connect(media_controler,&SparkMediaControler::onStatusChange,[=](){
        // PageData data = PageData{"新页面","Content",QUrl(""),"",Box};
        // addMediaPage(data);
        if (!media_controler->getStatus())
        {
            play_button->setIcon(play_button->style()->standardIcon(DStyle::SP_MediaPlay));
        } else
        {
            play_button->setIcon(play_button->style()->standardIcon(DStyle::SP_MediaPause));
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
        media_controler->previousmedia();
    });
    next_play = new DPushButton(controlers); // 初始化下一个按钮
    next_play->setIcon(next_play->style()->standardIcon(DStyle::SP_MediaSeekForward));
    next_play->setIconSize(QSize(40,40));
    next_play->setFixedSize(40,40);
    next_play->setStyleSheet(".QPushButton{background-color:rgba(196, 189, 189, 0); border-radius: 20px;}\
                                .QPushButton:hover{background-color:rgba(196, 189, 189, 0.2); border-radius: 20px;}\
                                .QPushButton:pressed{background-color:rgba(196, 189, 189, 0.3); border-radius: 20px;}");
    connect(next_play,&DPushButton::clicked,[=](){
        media_controler->nextMedia();
    });

    controler_box_layout->addWidget(previous_play);
    controler_box_layout->addWidget(play_button);
    controler_box_layout->addWidget(next_play);

    fullscreen_button = new DPushButton("小窗",controlers); // 初始化全屏按钮
    fullscreen_button->setFixedSize(90,50);
    fullscreen_button->move(10,80-50-5);
    
    title_bar->raise(); // 置顶 titlebar
    controlers->raise(); // 置顶 controlers

    speed_box = new SpeedBox(controlers); // 配置播放速度
    volume_box = new VolumeBox(controlers); // 配置音量按键
    play_list_button = new PlayListButton(controlers); // 配置播放列表
    ai_button = new DPushButton("AI",controlers); // 配置AI按钮
    ai_button->setFixedSize(30,30);
    ai_button->move(width()-30-105,35);
    // test = new DPushButton(controlers);
    // connect(test,&DPushButton::clicked,this,[&](){
    //     SparkMediaControler::getInstance()->setPlaybackSpeed(1.5);
    // });
    playmode_button = new DPushButton(controlers);
    playmode_button->setText("顺序播放");
    playmode_button->setFixedSize(75,30);
    playmode_button->move(width()-30-185,35);
    video_box = new VideoBox(this);
    video_box->raise();
}

void Sparkplayer::resizeEvent(QResizeEvent *event)
{
    DMainWindow::resizeEvent(event);
    title_bar->setFixedWidth(event->size().width());
    video_box->resizeEvent();
    controlers->setFixedWidth(width());
    controlers->move(0,height()-80);
    volume_box->resizebox();
    speed_box->resizebox();
    play_list_button->resizeEvent(event);
    ai_button->move(width()-30-105,35);
    playmode_button->move(width()-30-185,35);
}

void Sparkplayer::reloadMediaPage()
{
    // TODO 优化删除逻辑
    int index = 0;
    
    for(QList<PageData>::iterator data = page_data.begin();data < page_data.end();data++){
        PageData *pdata = &(*data);
        int i =media_list_buttons->buttons().size();
        if (media_list_buttons->buttons().size()<=index) {
            MediaListButton *b = new MediaListButton(QString("%1").arg(pdata->title),media_list_context);
            b->setFixedHeight(50);
            b->setFixedWidth(180);
            b->setCheckable(true);
            b->setIcon(Path::applicationPath("images/icon.png").toString());
            if (pdata->list_type == Box)
            {
                QTimer::singleShot(100,this,[b](){
                QDir dir(b->getData()->path);
                QStringList fileList = dir.entryList(QDir::Files);
                    if (!fileList.isEmpty()) {
                        QString filePath = dir.absoluteFilePath(fileList.first());
                        int w = Codec::getTitleImgWidth(fs::path(filePath.toStdString()));
                        int h = Codec::getTitleImgHeight(fs::path(filePath.toStdString()));
                        QSize size(w,h);
                        QImage img(size,QImage::Format_RGB32);
                        uint8_t* imgdata[1] = { reinterpret_cast<uint8_t*>(img.bits()) };
                        int linesize[1] = { static_cast<int>(img.bytesPerLine()) };
                        int res = Codec::getTitleImg(fs::path(filePath.toStdString()),size.width(),size.height(),imgdata,linesize);
                        if (res >= 0) {
                            b->setIcon(ImageTools::toPixmap(img,{b->height()-10,b->height()-10},6));
                        }
                    }
                });
            }
            b->setData(pdata);
            b->setStyleSheet(".QPushButton{background-color:rgba(196, 189, 189, 0); border-radius: 10px;}\
                                .QPushButton:hover{background-color:rgba(196, 189, 189, 0.2); border-radius: 10px;}\
                                .QPushButton:pressed{background-color:rgba(196, 189, 189, 0.3); border-radius: 10px;}\
                                .QPushButton:checked{background-color:rgb(100, 180, 255); border-radius: 10px;}");
            media_list_buttons->addButton(b);
            media_list_context_layout->insertWidget(index,b);
            if (index == 0)
            {
                b->click();
            }
        }
        MediaListButton *pButton = (MediaListButton *)(media_list_buttons->buttons()[index]);
        PageData *d=pButton->getData();
        if (pdata != pButton->getData())
        {
            pButton->setData(pdata);
        }
        index++;
    }
    for (auto i = media_list_buttons->buttons().begin()+index; i < media_list_buttons->buttons().end(); i++)
    {  
        (*i)->deleteLater();
        media_list_buttons->removeButton((*i));
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

void Sparkplayer::removeMediaPage(MediaListButton *b)
{
    for (QList<PageData>::iterator d = page_data.begin(); d != page_data.end(); d++) {
        if (d->path == ((MediaListButton *)b)->getData()->path)
        {
            page_data.erase(d);
            break;
        }
        
    }

    reloadMediaPage();
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

void Sparkplayer::hideControlers(bool t)
{
    QPropertyAnimation *animation = new QPropertyAnimation(controlers,"geometry");
    animation->setDuration(300);
    if (t)
    {
        animation->setStartValue(controlers->geometry());
        animation->setEndValue(QRect(0,height(),controlers->width(),80));
    } else {

        animation->setStartValue(controlers->geometry());
        animation->setEndValue(QRect(0,height()-80,controlers->width(),80));
    }
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void Sparkplayer::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        for (auto b : media_list_buttons->buttons()) {
            int index = media_list_buttons->buttons().indexOf(b);
            if (b->underMouse()&&index>2)
            {
                QMenu contextMenu(this);
                contextMenu.addAction(tr("删除媒体库"), this, [this,b](){
                    removeMediaPage((MediaListButton *)b);
                    std::list<fs::path> paths;
                    int index = 0;
                    for (auto i : page_data) {
                        if (index>=3) paths.push_back(fs::path(i.path.toStdString()));
                        index++;
                    }
                    SparkSettings::getInstance()->setMediaLibraries(paths);
                    
                });
                contextMenu.exec(event->globalPos());
                break;
            }
            
        }
        if (media_list_context->underMouse())
        {
            QMenu contextMenu(this);
            contextMenu.addAction(tr("添加媒体库"), this, [this](){
                QString dir = QFileDialog::getExistingDirectory(this, tr("选择媒体目录"), QDir::homePath());
                if (!dir.isEmpty()) {
                    QString title = QFileInfo(dir).fileName();
                    addMediaPage(PageData{ title, "", dir, "", Box }, -1);

                    std::list<fs::path> paths;
                    int index = 0;
                    for (auto i : page_data) {
                        if (index>=3) paths.push_back(fs::path(i.path.toStdString()));
                        index++;
                    }
                    SparkSettings::getInstance()->setMediaLibraries(paths);
                
                }
            });
            contextMenu.exec(event->globalPos());
        }
        
    } else {
        DMainWindow::mousePressEvent(event);
    }
}

void Sparkplayer::slotFullscreen(bool t)
{
    if (t)
    {
        title_bar->raise();
        title_bar->setStyleSheet(".QWidget{background-color:rgba(128, 128, 128, 0.2);}");
        controlers->raise();
        controlers->setStyleSheet(".QWidget{background-color:rgba(128, 128, 128, 0.2);}");
        volume_box->raise();
    } else {
        video_box->raise();
        controlers->setStyleSheet(".QWidget{background-color:rgba(56, 56, 56, 0);}");
        title_bar->setStyleSheet(".QWidget{background-color:rgba(0, 0, 0, 0);}");
    }
    
}

void Sparkplayer::slotChangePlayMode()
{
    PlayMode mode = PlayMode((SparkMediaControler::getInstance()->getPlayMode()+1)%4);
    SparkMediaControler::getInstance()->setPlayMode(mode);
    switch (mode)
    {
        case PlayMode_List:
            playmode_button->setText("顺序播放");
            break;
        case PlayMode_Random:
            playmode_button->setText("随机播放");
            break;
        case PlayMode_One:
            playmode_button->setText("单曲播放");
            break;
        case PlayMode_OneLoop:
            playmode_button->setText("单曲循环");
            break;
        default:
            break;
    }
    qDebug() << "playmode changed to " << mode;
}

void Sparkplayer::slotAI()
{
    if (!SparkMediaControler::getInstance()->isHaveFile()){
        return;
    }
    QMenu contextMenu(this);
    if (SparkMediaControler::getInstance()->isVideo()) {
        contextMenu.addAction("动画角色识别", this, [this](){
            video_box->SparkAI(1);
        });
        contextMenu.addAction("表情识别", this, [this](){
            video_box->SparkAI(0);
        });
    } else {
        contextMenu.addAction("音频识别", this, [this](){
            video_box->SparkAI(2);
        });
    }
    //ai_button在屏幕的位置
    QPoint pos = ai_button->mapToGlobal(QPoint(0, 0));
    pos.setX(pos.x()-(contextMenu.sizeHint().width()-ai_button->width())/2);
    pos.setY(pos.y()-contextMenu.sizeHint().height());
    contextMenu.exec(pos);
}

void Sparkplayer::slotPlay()
{
    play_button->click();
}

void Sparkplayer::slotNext()
{
    next_play->click();
}

void Sparkplayer::slotPrevious()
{
    previous_play->click();
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
        bottom->setStyleSheet(".QWidget{background-color: #fafafa;}");

        ai_button->setStyleSheet(".QPushButton{ color:black; background-color:rgba(196, 189, 189, 0); border-radius: 10px;}\
                                    .QPushButton:hover{ color:black; background-color:rgba(196, 189, 189, 0.2); border-radius: 10px;}\
                                    .QPushButton:pressed{ color:black; background-color:rgba(196, 189, 189, 0.3); border-radius: 10px;}");

        playmode_button->setStyleSheet(".QPushButton{ color:black; background-color:rgba(196, 189, 189, 0.3); border-radius: 10px;}\
                                        .QPushButton:hover{ color:black; background-color:rgba(196, 189, 189, 0.1); border-radius: 10px;}\
                                        .QPushButton:pressed{ color:black; background-color:rgba(196, 189, 189, 0.2); border-radius: 10px;}");


        fullscreen_button->setStyleSheet(".QPushButton{ color:black; background-color:rgba(165, 165, 165, 0.1); border-radius: 10px;}\
                                    .QPushButton:hover{ color:black; background-color:rgba(196, 189, 189, 0.2); border-radius: 10px;}\
                                    .QPushButton:pressed{ color:black; background-color:rgba(196, 189, 189, 0.3); border-radius: 10px;}");
    } else {
        title->setStyleSheet(".QLabel{background-color: #1a1a21;}");
        media_list_context->setStyleSheet(".QWidget{background-color: #1a1a21;}");
        media_list->setStyleSheet(".QScrollArea{background-color: #1a1a21;border-style: none;}");
        main_box->setStyleSheet(".QWidget{background-color: #13131a;}");
        bottom->setStyleSheet(".QWidget{background-color: #2d2d38;}");

        ai_button->setStyleSheet(".QPushButton{ color:white; background-color:rgba(196, 189, 189, 0); border-radius: 10px;}\
                                    .QPushButton:hover{ color:white; background-color:rgba(196, 189, 189, 0.2); border-radius: 10px;}\
                                    .QPushButton:pressed{ color:white; background-color:rgba(196, 189, 189, 0.3); border-radius: 10px;}");
            
        playmode_button->setStyleSheet(".QPushButton{ color:white; background-color:rgba(196, 189, 189, 0.3); border-radius: 10px;}\
                                        .QPushButton:hover{ color:white; background-color:rgba(196, 189, 189, 0.1); border-radius: 10px;}\
                                        .QPushButton:pressed{ color:white; background-color:rgba(196, 189, 189, 0.2); border-radius: 10px;}");
        fullscreen_button->setStyleSheet(".QPushButton{ color:white; background-color:rgba(165, 165, 165, 0.1); border-radius: 10px;}\
                                    .QPushButton:hover{ color:white; background-color:rgba(196, 189, 189, 0.2); border-radius: 10px;}\
                                    .QPushButton:pressed{ color:white; background-color:rgba(196, 189, 189, 0.3); border-radius: 10px;}");
    }
    
}
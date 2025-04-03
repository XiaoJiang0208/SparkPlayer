#include "MainPage.h"

MainPage::MainPage(QWidget *parent, PageData *data)
    : DWidget(parent)
{
    if (data)
    {
        this->data = data;
    }else
    {
        this->data = new PageData;
    }
    
    initUI();
    reloadMedia();

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &MainPage::slotThemeTypeChanged);
    // 播放全部
    connect(play_all, &DPushButton::clicked, this, [&](){
        SparkMediaControler::getInstance()->pause();
        SparkMediaControler::getInstance()->removeAllMedia();
        for (auto d : media_data_list){
            SparkMediaControler::getInstance()->addMedia(d);
        }
        SparkMediaControler::getInstance()->play();
    });
}

void MainPage::initUI()
{   
    this->setLayout(new QVBoxLayout(this)); //设置布局
    this->layout()->setMargin(0);
    this->layout()->setSpacing(0);

    
    // 初始化媒体列表
    media_list = new DScrollArea(this);
    media_list->setWidgetResizable(true);  // 新增，确保内容可自适应横向宽度
    media_list->setLayout(new QVBoxLayout(media_list));
    this->layout()->addWidget(media_list);
    media_list_context = new DWidget(media_list);
    media_list_context->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred); // 新增，横向铺满
    media_list_context->setStyleSheet(".QWidget{background-color: rgba(0, 0, 0, 0);}");
    media_list->layout()->addWidget(media_list_context);
    media_list_context_layout = new QVBoxLayout(media_list_context);
    media_list_context_layout->setContentsMargins(20, 0, 20, 0);
    media_list_context_layout->setSpacing(0);
    media_list_context_layout->setAlignment(Qt::AlignTop);  // 新增，确保从上到下排列
    
    // 初始化主标题栏
    media_list_bar = new DWidget(media_list_context);
    media_list_bar->setFixedHeight(100);
    media_list_bar_layout = new QHBoxLayout(media_list_bar);
    media_list_bar_layout->setAlignment(Qt::AlignLeft);
    media_list_context_layout->addWidget(media_list_bar);

    // 添加图标
    media_list_bar_icon = new DLabel(media_list_bar);
    media_list_bar_icon->setPixmap(ImageTools::toPixmap(Path::applicationPath("images/bg.png").toString(), QSize(100, 80),15));
    media_list_bar_icon->show();
    media_list_bar_layout->addWidget(media_list_bar_icon);

    media_list_bar_right_layout = new QVBoxLayout();
    media_list_bar_layout->addLayout(media_list_bar_right_layout);

    // 添加标题
    media_list_bar_title = new DLabel(data->title,media_list_bar);
    media_list_bar_title->setFont(DFontSizeManager::instance()->t3());
    media_list_bar_title->setWordWrap(true);
    media_list_bar_title->setAlignment(Qt::AlignLeft);
    media_list_bar_title->show(); 
    media_list_bar_right_layout->addWidget(media_list_bar_title);

    media_list_bar_button_layout = new QHBoxLayout();
    media_list_bar_button_layout->setAlignment(Qt::AlignLeft);
    media_list_bar_right_layout->addLayout(media_list_bar_button_layout);

    play_all = new DPushButton("播放全部",media_list_bar);
    play_all->setIcon(play_all->style()->standardIcon(DStyle::SP_MediaPlay));
    play_all->setIconSize(QSize(15,15));
    play_all->setFixedSize(90,30);
    media_list_bar_button_layout->addWidget(play_all);

    
    media_list->setWidget(media_list_context);

    slotThemeTypeChanged();
}

void MainPage::reloadMedia()
{
    fs::path path = data->path.toStdString();
    if (!fs::directory_entry(path).exists()) {qWarning()<<"目录不存在";return;}
    for (const auto& entry : fs::directory_iterator(path)) {
        if (!entry.is_directory())
        {
            media_data_list.push_back(entry.path());
        }
    }
    for (QList<fs::path>::iterator data = media_data_list.begin();data < media_data_list.end();data++) {
        MediaBox *b = new MediaBox(*data,media_list_context);
        b->setFixedHeight(50);
        media_list_context_layout->addWidget(b);
        b->autoSetIcon();
    }
}

void MainPage::slotThemeTypeChanged()
{ 
    play_all->setStyleSheet(".QPushButton{background-color:rgb(100, 180, 255); border-radius: 10px;}\
                             .QPushButton:hover{background-color:rgb(74, 143, 207); border-radius: 10px;}\
                             .QPushButton:pressed{background-color:rgb(117, 163, 206); border-radius: 10px;}");
    media_list_bar->setStyleSheet(".QWidget { border-bottom: 2px solid #000; }");
    media_list_bar_icon->setStyleSheet(".QLabel{\
        background-color: rgba(197, 37, 37, 0.37);\
        backdrop-filter: blur( 1.5px );}");
    media_list->setStyleSheet(".QScrollArea{border-style: none;background-color:rgba(0, 0, 0, 0);}");
    media_list_bar->setStyleSheet(".QWidget{background-color:rgba(0, 0, 0, 0);}");
    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType){
    } else {

    }
    // Implement the logic for theme type change
}
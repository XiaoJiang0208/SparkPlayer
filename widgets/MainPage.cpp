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
    
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &MainPage::slotThemeTypeChanged);
    initUI();
    reloadMedia();
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
    
    //初始化主标题栏
    media_list_bar = new DWidget(media_list_context);
    media_list_bar->setFixedHeight(100);
    media_list_bar_layout = new QHBoxLayout(media_list_bar);
    media_list_bar_layout->setAlignment(Qt::AlignLeft);

    media_list_context_layout->addWidget(media_list_bar);

    media_list_bar_icon = new DLabel(media_list_bar);
    media_list_bar_icon->setPixmap(ImageTools::toPixmap(Path::applicationPath("images/bg.png").toString(), QSize(100, 80),15));
    media_list_bar_icon->show();
    media_list_bar_layout->addWidget(media_list_bar_icon);

    media_list_bar_title = new DLabel(data->title,media_list_bar);
    media_list_bar_title->setFont(DFontSizeManager::instance()->t3());
    media_list_bar_title->show(); 
    media_list_bar_layout->addWidget(media_list_bar_title);


    media_box_list = new QButtonGroup(media_list_context);
    
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
        b->setCheckable(true);
        // TODO 优化图片加载性能
        QSize size(b->getIconSize());
        QImage img(size,QImage::Format_RGB32);
        uint8_t* imgdata[1] = { reinterpret_cast<uint8_t*>(img.bits()) };
        int linesize[1] = { static_cast<int>(img.bytesPerLine()) };
        int res = Codec::getTitleImg(b->getMediaPath(),size.width(),size.height(),imgdata,linesize);
        if (res >= 0) {
            b->setIcon(ImageTools::toPixmap(img,{(b->height()-10)/9*16,b->height()-10},6));
        } else {
            b->setIcon(ImageTools::toPixmap(Path::applicationPath("images/icon.png").toString(),{(b->height()-10)/9*16,b->height()-10},6));
        }
        media_box_list->addButton(b);
        media_list_context_layout->addWidget(b);
        if (b->getMediaPath() == SparkMediaControler::getInstance()->getPath())
        {
            b->setChecked(true);
        }
        
    }
}

void MainPage::slotThemeTypeChanged()
{ 
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
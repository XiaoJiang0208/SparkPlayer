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
    if (data->list_type == List) this->setAcceptDrops(true);
    initUI();
    
    QTimer::singleShot(10,this,[&](){
        QDir dir(this->data->path);
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
                media_list_bar_icon->setPixmap(ImageTools::toPixmap(img,media_list_bar_icon->size(),6));
            }
        }
    });
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
    media_list_bar_icon->setPixmap(ImageTools::toPixmap(Path::applicationPath("images/icon.png").toString(), QSize(100, 80),15));
    media_list_bar_icon->show();
    media_list_bar_layout->addWidget(media_list_bar_icon);

    media_list_bar_right_layout = new QVBoxLayout();
    media_list_bar_layout->addLayout(media_list_bar_right_layout);

    // 添加标题
    media_list_bar_title = new DLabel(data->list_type==Box ? data->title : "拖到此处创建列表",media_list_bar);
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
    if (data->list_type == Box) {
        fs::path path = data->path.toStdString();
        if (!fs::directory_entry(path).exists()) {qWarning()<<"目录不存在";return;}
        for (const auto& entry : fs::directory_iterator(path)) {
            if (!entry.is_directory())
            {
                media_data_list.push_back(entry.path());
            }
        }
    }
    while (!media_box_list.empty()){
        QWidget *b = media_box_list.back();
        media_list_context_layout->removeWidget(b);
        b->deleteLater();
        media_box_list.pop_back();
    }
    for (QList<fs::path>::iterator data = media_data_list.begin();data < media_data_list.end();data++) {
        MediaBox *b = new MediaBox(*data,media_list_context);
        b->setFixedHeight(50);
        media_list_context_layout->addWidget(b);
        b->autoSetIcon();
        media_box_list.push_back(b);
    }
}

void MainPage::dropEvent(QDropEvent *event)
{
    // 放下事件
    const QMimeData *mimeData = event->mimeData();  // 获取MIME数据
    if(mimeData->hasUrls())  // 如果数据中包含URL
    {
        QList<QUrl> urlList = mimeData->urls();  // 获取URL列表
        // 将其中的第一个URL表示为本地文件路径
        QString fileName = urlList.at(0).toLocalFile();  // toLocalFile()转换未本地文件路径
        qDebug() << "fileName:" << fileName;
        if(!fileName.isEmpty())
        {
            // 文件路径不为空
            media_data_list.append(fs::path(fileName.toStdString()));
            reloadMedia();
        }
    }
}

void MainPage::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())  // 数据中是否包含URL
    {
        event->acceptProposedAction();  // 如果数据中包含URL，就接收动作
    }
    else
    {
        event->ignore();  // 如果数据中不包含URL，就忽略该事件
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
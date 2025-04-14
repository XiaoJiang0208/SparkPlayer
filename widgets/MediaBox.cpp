#include "MediaBox.h"

MediaBox::MediaBox(fs::path &path, QWidget *parent, bool is_del_mod):QPushButton(parent){
    media_path = path;
    menu = nullptr;

    initUI();
    QString name = QString::fromStdString(media_path.stem().string());
    setText(name);
    setDelMode(is_del_mod);
    if (SparkMediaControler::getInstance()->getPath() == this->media_path)
    {
        m_name->setStyleSheet(".QLabel{color:rgb(100, 180, 255);}");
    } else {
        m_name->setStyleSheet(".QLabel {}");
    }
    connect(SparkMediaControler::getInstance(), &SparkMediaControler::onFileOpen,this, [&](){
        if (SparkMediaControler::getInstance()->getPath() == this->media_path)
        {
            m_name->setStyleSheet(".QLabel{color:rgb(100, 180, 255);}");
        } else {
            m_name->setStyleSheet(".QLabel {}");
        }
    });

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &MediaBox::slotThemeTypeChanged);
    slotThemeTypeChanged();

    icon->setFixedSize(this->getIconSize());
}

MediaBox::~MediaBox() {
}

void MediaBox::initUI()
{
    setLayout(new QHBoxLayout());
    layout()->setMargin(5);
    this->icon = new QLabel(this);
    this->icon->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    this->icon->setFixedSize(0,0);
    layout()->addWidget(this->icon);
    this->m_name = new QLabel(this);
    this->m_name->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    layout()->addWidget(this->m_name);
    //this->m_name->setStyleSheet(".QLabel{background: black;}");
    this->m_name->setWordWrap(true);
    this->m_name->setAlignment(Qt::AlignCenter);
}

void MediaBox::setIcon(const QPixmap &icon)
{
    int w =(this->height()-10)/9*16;
    int h =this->height()-10;
    this->icon->setFixedSize(w,h);
    this->icon->setPixmap(icon);
    this->m_name->setAlignment(Qt::AlignLeft);
}

void MediaBox::autoSetIcon()
{
        this->setIcon(ImageTools::toPixmap(Path::applicationPath("images/icon.png").toString(),this->getIconSize(),6));
        // TODO 优化图片加载性能
        // QtConcurrent::run([this]() {
        //     QSize size(this->getIconSize());
        //     QImage img(size,QImage::Format_RGB32);
        //     uint8_t* imgdata[1] = { reinterpret_cast<uint8_t*>(img.bits()) };
        //     int linesize[1] = { static_cast<int>(img.bytesPerLine()) };
        //     int res = Codec::getTitleImg(this->getMediaPath(),size.width(),size.height(),imgdata,linesize);
        //     if (res >= 0) {
        //         QPixmap pixmap = ImageTools::toPixmap(img, {(this->height()-10)/9*16, this->height()-10}, 6);
        //         QMetaObject::invokeMethod(this, [this, pixmap]() {
        //             this->setIcon(pixmap);
        //         }, Qt::QueuedConnection);
        //     }
        // });
        // 提前缓存数据
        QSize iconSize = this->getIconSize();
        fs::path mediaPath = this->getMediaPath();
        int widgetHeight = this->height();
        QFutureWatcher<QImage>* watcher = new QFutureWatcher<QImage>(this);

        // 连接完成信号
        connect(watcher, &QFutureWatcher<QImage>::finished, this, [this, watcher, widgetHeight]() {
            QImage img = watcher->result();
            if (!img.isNull()) {
                QPixmap pixmap = ImageTools::toPixmap(img, {(widgetHeight-10)/9*16, widgetHeight-10}, 6);
                this->setIcon(pixmap);
            }
            watcher->deleteLater();
        });
        QFuture<QImage> future = QtConcurrent::run([](const QSize& iconSize, const fs::path& mediaPath){
            QImage img(iconSize, QImage::Format_RGB32);
            uint8_t* imgdata[1] = { reinterpret_cast<uint8_t*>(img.bits()) };
            int linesize[1] = { static_cast<int>(img.bytesPerLine()) };
            int res = Codec::getTitleImg(mediaPath, iconSize.width(), iconSize.height(), imgdata, linesize);
            return (res >= 0) ? img : QImage();
        }, iconSize, mediaPath);
        watcher->setFuture(future);
}

void MediaBox::setText(const QString &text)
{
    m_name->setText(text);
}

QString MediaBox::text() const
{
    return m_name->text();
}

fs::path MediaBox::getMediaPath()
{
    return media_path;
}

QSize MediaBox::getIconSize()
{
    return QSize((height()-10)/9*16,height()-10);
}

void MediaBox::setDelMode(bool is)
{
    del_mode=is;
    if (menu)
    {
        menu->deleteLater();
        menu = nullptr;
    }
    
    if (del_mode) {
        menu = new QMenu();
        QAction *nextplay = menu->addAction("从播放列表移除");

        connect(nextplay, &QAction::triggered, this, [&]()
        {
            SparkMediaControler::getInstance()->removeMedia(this->getMediaPath());
        });
    } else {
        menu = new QMenu();
        QAction *nextplay = menu->addAction("下一个播放");

        connect(nextplay, &QAction::triggered, this, [&]()
        {
            SparkMediaControler::getInstance()->addMedia(media_path,1);
            if (SparkMediaControler::getInstance()->getPlayList()->size() == 1) {
                SparkMediaControler::getInstance()->play();
            }
        });
    }
    
}

void MediaBox::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
        SparkMediaControler::getInstance()->pause();
        SparkMediaControler::getInstance()->addMedia(media_path,1);
        SparkMediaControler::getInstance()->nextMedia();
        SparkMediaControler::getInstance()->play();
    } else if (ev->button() == Qt::RightButton) {
        menu->exec(QCursor::pos());
    }
    
}

void MediaBox::slotThemeTypeChanged() {
    icon->setStyleSheet(".QLabel{background-color:rgba(0,0,0,0);}");
    this->setStyleSheet(".MediaBox{background-color:rgba(255, 255, 255, 0); border-radius: 10px;}\
                         .MediaBox:hover{background-color:rgba(138, 138, 138, 0.2); border-radius: 10px;}");
    // 添加阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(menu);
    shadow->setBlurRadius(5);    // 模糊半径
    shadow->setOffset(0, 0);        // 偏移量
    shadow->setColor(Qt::black);    // 阴影颜色
    menu->setGraphicsEffect(shadow);
    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
        menu->setStyleSheet(".QMenu { color: black; background-color: #ffffff; border-radius: 2px; }\
                             .QMenu::item {  color: black; background-color: rgba(255, 255, 255, 0); }\
                             .QMenu::item:hover{ color: black; color:rgb(41, 41, 41); background-color:rgb(138, 138, 138); }\
                             .QMenu::item:pressed{ color: black; background-color:rgb(138, 138, 138); }");
    } else {
        menu->setStyleSheet(".QMenu { color: white; background-color: #2d2d38; border-radius: 2px; }\
                             .QMenu::item { color: white; background-color: rgba(255, 255, 255, 0); }\
                             .QMenu::item:hover{ color: white; background-color:rgba(138, 138, 138, 0.2); }\
                             .QMenu::item:pressed{ color: white; background-color:rgba(138, 138, 138, 0.3); }");
    } 
    
    
}
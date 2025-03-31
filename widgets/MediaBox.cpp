#include "MediaBox.h"

MediaBox::MediaBox(fs::path &path, QWidget *parent, bool is_del_mod):QPushButton(parent){
    media_path = path;
    menu = nullptr;

    initUI();
    QString name = QString::fromStdString(media_path.stem().string());
    setText(name);
    setDelMode(is_del_mod);
    connect(SparkMediaControler::getInstance(), &SparkMediaControler::onFileOpen,this, [&](){
        if (SparkMediaControler::getInstance()->getPath() == this->media_path)
        {
            this->setChecked(true);
        }
    });

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &MediaBox::slotThemeTypeChanged);
    slotThemeTypeChanged();
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

void MediaBox::checkStateSet()
{
    if (isChecked()){
        m_name->setStyleSheet(".QLabel{color:rgb(100, 180, 255);}");
    } else {
        m_name->setStyleSheet(".QLabel {}");
    }
}

void MediaBox::slotThemeTypeChanged() {
    this->setStyleSheet(".MediaBox{background-color:rgba(255, 255, 255, 0); border-radius: 10px;}\
                         .MediaBox:hover{background-color:rgba(138, 138, 138, 0.2); border-radius: 10px;}");
    // 添加阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(menu);
    shadow->setBlurRadius(5);    // 模糊半径
    shadow->setOffset(0, 0);        // 偏移量
    shadow->setColor(Qt::black);    // 阴影颜色
    menu->setGraphicsEffect(shadow);
    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
        menu->setStyleSheet(".QMenu { background-color: #ffffff; border-radius: 2px; }\
                             .QMenu::item {  color: black; background-color: rgba(255, 255, 255, 0); }\
                             .QMenu::item:hover{ color: black; color:rgb(41, 41, 41); background-color:rgb(138, 138, 138); }\
                             .QMenu::item:pressed{ color: black; background-color:rgb(138, 138, 138); }");
    } else {
        menu->setStyleSheet(".QMenu { background-color: #2d2d38; border-radius: 2px; }\
                             .QMenu::item { color: black; background-color: rgba(255, 255, 255, 0); }\
                             .QMenu::item:hover{ color: black; background-color:rgba(138, 138, 138, 0.2); }\
                             .QMenu::item:pressed{ color: black;background-color:rgba(138, 138, 138, 0.3); }");
    } 
    
    
}
#include "MediaBox.h"

MediaBox::MediaBox(fs::path &path, QWidget *parent):QPushButton(parent){
    media_path = path;
    initUI();
    QString name = QString::fromStdString(media_path.stem().string());
    setText(name);
}

MediaBox::~MediaBox(){
}

void MediaBox::initUI()
{
    setLayout(new QHBoxLayout());
    layout()->setMargin(5);
    this->icon = new QLabel(this);
    this->icon->setFixedSize(0,0);
    layout()->addWidget(this->icon);
    this->m_name = new QLabel(this);
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

void MediaBox::nextCheckState()
{
    QPushButton::nextCheckState();
    
    if (isChecked()){
        m_name->setStyleSheet(".QLabel{color:rgb(100, 180, 255)}");
    } else {
        m_name->setStyleSheet(".QLabel{}");
    }
}


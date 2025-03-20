#include "MediaListButton.h"

MediaListButton::MediaListButton(QWidget *parent) : QPushButton(parent)
{
    initUI();
}

MediaListButton::MediaListButton(const QString &text, QWidget *parent) : QPushButton(parent)
{
    initUI();
    setText(text);
}

MediaListButton::MediaListButton(const QIcon &icon, const QString &text, QWidget *parent) : QPushButton(parent)
{
    initUI();
    setText(text);
    setIcon(icon);
}



MediaListButton::~MediaListButton()
{
}

void MediaListButton::initUI()
{
    setLayout(new QHBoxLayout());
    layout()->setMargin(5);
    this->icon = new QLabel(this);
    this->icon->setFixedSize(0,0);
    layout()->addWidget(this->icon);
    this->m_text = new QLabel(this);
    layout()->addWidget(this->m_text);
    //this->m_text->setStyleSheet(".QLabel{background: black;}");
    this->m_text->setWordWrap(true);
    this->m_text->setAlignment(Qt::AlignCenter);
}

void MediaListButton::setIcon(const QPixmap &icon)
{
    int bd =this->height()-10;
    this->icon->setFixedSize(bd,bd);
    this->icon->setPixmap(icon);
    this->m_text->setAlignment(Qt::AlignLeft);
}

void MediaListButton::setIcon(const QString &iconurl)
{

    int bd =this->height()-10;
    setIcon(ImageTools::toPixmap(iconurl,QSize(bd,bd),6));
}

void MediaListButton::setIcon(const QIcon &icon)
{

    int bd =this->height()-10;
    setIcon(icon.pixmap(icon.actualSize(QSize(bd, bd))));
}

void MediaListButton::setText(const QString &text)
{
    m_text->setText(text);
}

QString MediaListButton::text() const
{
    return m_text->text();
}

void MediaListButton::setData(PageData *data)
{
    page_data=data;
    setText(page_data->title);
}

PageData *MediaListButton::getData()
{
    return page_data;
}


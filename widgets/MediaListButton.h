#ifndef MEDIALISTBUTTON_H
#define MEDIALISTBUTTON_H

#include "../include.h"
#include "../utils/ImageTools.h"
#include "MainPage.h"

class MediaListButton : public QPushButton
{
private:
    QLabel *icon;
    QLabel *m_text;

    PageData *page_data;
public:
    explicit MediaListButton(QWidget *parent = nullptr);
    explicit MediaListButton(const QString &text, QWidget *parent = nullptr);
    MediaListButton(const QIcon& icon, const QString &text, QWidget *parent = nullptr);
    ~MediaListButton();

    void initUI();

    void setIcon(const QPixmap &icon);
    void setIcon(const QString &iconurl);
    void setIcon(const QIcon &icon);

    void setText(const QString &text);
    QString text() const;

    void setData(PageData *data);
    PageData *getData();

    //void resizeEvent(QResizeEvent *event);
};


#endif // MEDIALISTBUTTON_H
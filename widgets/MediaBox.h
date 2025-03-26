#ifndef MEDIABOX_H
#define MEDIABOX_H

#include <filesystem>

#include "../include.h"
#include "MainPage.h"
#include "../utils/Codec.h"

namespace fs = std::filesystem;

class MediaBox : public QPushButton
{
    Q_OBJECT
private:
    fs::path media_path;

    QLabel *icon;
    QLabel *m_name;
    QLabel *m_time;
    
public:
    MediaBox(fs::path &path, QWidget *parent = nullptr);
    ~MediaBox();

    void initUI();

    void setIcon(const QPixmap &icon);

    void setText(const QString &text);
    QString text() const;

    fs::path getMediaPath();
    
    QSize getIconSize();

    void nextCheckState();

    void mousePressEvent(QMouseEvent *ev);
};


#endif
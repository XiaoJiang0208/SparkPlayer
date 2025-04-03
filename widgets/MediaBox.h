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

    QMenu *menu;
    
    bool del_mode;
public:
    MediaBox(fs::path &path, QWidget *parent = nullptr, bool is_del_mod = 0);
    ~MediaBox();

    void initUI();

    void setIcon(const QPixmap &icon);
    void autoSetIcon();

    void setText(const QString &text);
    QString text() const;

    fs::path getMediaPath();
    
    QSize getIconSize();

    void setDelMode(bool is);

    void mouseReleaseEvent(QMouseEvent *ev);

public Q_SLOTS:
    void slotThemeTypeChanged();
};


#endif
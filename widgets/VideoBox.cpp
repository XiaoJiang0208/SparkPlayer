#include "VideoBox.h"

VideoBox::VideoBox(QWidget *parent) : QLabel(parent), hideMouseTimer(new QTimer(this))
{
    
    resize(90,50);
    move(10,parent->height()-this->height()-10);
    //SparkMediaControler::getInstance()->setVideoSize(90,50);
    connect(SparkMediaControler::getInstance(),&SparkMediaControler::onImageDone,this,&VideoBox::showimg);
    connect(SparkMediaControler::getInstance(),&SparkMediaControler::onImageDone,this,[&](){
        clearMark();
    });
    hideMouseTimer->setSingleShot(true);
    hideMouseTimer->setInterval(3000); // 3秒无操作后隐藏鼠标
    connect(hideMouseTimer, &QTimer::timeout, this, [this](){
        this->setCursor(Qt::BlankCursor);
    });
    raise();
    //setPixmap(ImageTools::toPixmap(Path::applicationPath("images/icon.png").toString(),size,6));
}

VideoBox::~VideoBox()
{
}

void VideoBox::fullscreen(bool is)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this,"geometry");
    //SparkMediaControler::getInstance()->setVideoSize(parentWidget()->width(),parentWidget()->height());
    
    if (is)
    {
        animation->setStartValue(this->geometry());
        animation->setEndValue(QRect(0,0,parentWidget()->width(),parentWidget()->height()));
        animation->setEndValue(QRect(0,0,parentWidget()->width(),parentWidget()->height()));
    } else {
        animation->setStartValue(this->geometry());
        animation->setEndValue(QRect(10,parentWidget()->height()-50-5,90,50));
    }
    // TODO : 优化了显示效果没有优化播放时缩放性能
    //media_controler->setVideoSize(90,50);
    //if (!SparkMediaControler::getInstance()->getStatus()) { // 如果不在播放在重新渲染放大之后的当前帧   
    //    
    //}
    connect(animation, &QPropertyAnimation::valueChanged, this, &VideoBox::showimg);
    qDebug()<<"ww";
    animation->setDuration(200);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    is_fullscreen = is;
    this->setMouseTracking(is_fullscreen);
    hideMouseTimer->stop();
    this->setCursor(Qt::ArrowCursor);
    emit onFullscreen(is);
}

void VideoBox::clearMark()
{
    marks.clear();
    delete AIimg;
    AIimg = nullptr;
    showimg();
}

void VideoBox::addMark(int x, int y, int w, int h, const QString &text)
{
    marks.append({QRect(x,y,w,h),text});
}

void VideoBox::showMark()
{
    QPixmap orig = QPixmap::fromImage(*(SparkMediaControler::getInstance()->getImg()));
    if (orig.isNull())
        return;

    QPixmap overlay = orig.copy();
    QPainter painter(&overlay);
    painter.setRenderHint(QPainter::Antialiasing);

    // 画半透明背景
    QColor overlayColor(0, 0, 0, 128); // 50%透明黑色
    painter.fillRect(overlay.rect(), overlayColor);

    for (auto &mark : marks) {
        int x = mark.first.x();
        int y = mark.first.y();
        int w = mark.first.width();
        int h = mark.first.height();
        
        // 裁切指定区域的图像
        QPixmap cropped = orig.copy(x, y, w, h);
        // 将裁切的部分重绘回指定区域（去掉半透明效果）
        painter.drawPixmap(x, y, cropped);

        // 在裁切区域绘制文本
        painter.setPen(Qt::white);
        QFontMetrics fm(painter.font());
        QStringList lines = mark.second.split('\n');
        int textHeight = lines.size() * fm.height();
        //lines最长的长度
        int maxWidth = 0;
        for (const QString &line : lines) {
            int lineWidth = fm.horizontalAdvance(line);
            if (lineWidth > maxWidth) {
                maxWidth = lineWidth;
            }
        }
        QRect textRect(x-(maxWidth-w)/2, y + h, maxWidth, textHeight);
        painter.drawText(textRect, Qt::AlignCenter, mark.second);
    }

    painter.end();
    AIimg = new QPixmap(overlay);
    showimg();
}

void VideoBox::SparkAI(int type)
{
    if (SparkMediaControler::getInstance()->isVideo())
    {
        SparkMediaControler::getInstance()->pause();
    }
    fullscreen(true);
    QVariantAnimation *waveAnim = new QVariantAnimation(this);
    waveAnim->setDuration(2000);
    waveAnim->setStartValue(0);
    waveAnim->setEndValue(window()->width());
    waveAnim->setLoopCount(-1);
    connect(waveAnim, &QVariantAnimation::valueChanged, this, [this](const QVariant &val){
        QPixmap base = ImageTools::toPixmap(*(SparkMediaControler::getInstance()->getImg()), this->size());
        int offset = val.toInt();
        if (base.isNull())
            base = QPixmap(this->size());
        QPixmap overlay = base.copy();
        QPainter painter(&overlay);

        QLinearGradient gradient(offset-80, 0, offset + 80, 0);
        gradient.setColorAt(0.0, QColor(0, 0, 0, 0));
        gradient.setColorAt(1.0, QColor(100, 180, 255, 128));
        painter.fillRect(QRect(offset-80, 0, 80, this->height()), gradient);

        painter.end();
        setPixmap(overlay);
    });
    waveAnim->start();
    //等待200ms
    QTimer::singleShot(2000, this, [this,waveAnim,type](){
        QImage img = *(SparkMediaControler::getInstance()->getImg());
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        img.save(&buffer, "PNG");
        const char* imgChar = byteArray.constData();
        
        // 将耗时操作放入子线程
        QtConcurrent::run([this, byteArray, waveAnim, type](){
            std::string res;
            if (type==0) {
                res = SparkAIAPI::getInstance().FaceAPIEmotion(byteArray.constData(), byteArray.size());
            } else if (type==1) {
                res = SparkAIAPI::getInstance().FaceAPIAnimation(byteArray.constData(), byteArray.size());
            } else if (type==2) {
                res = SparkAIAPI::getInstance().MusicAPIStyle(SparkMediaControler::getInstance()->getPath().c_str());
            }
            
            // 返回主线程处理结果
            QMetaObject::invokeMethod(this, [this, waveAnim, res, type](){
                qDebug() << "emotion:" << QString::fromStdString(res);
                waveAnim->stop();
                waveAnim->deleteLater();
                if (type==0) {
                    SparkAIEmotion(QString::fromStdString(res));
                } else if (type==1) {
                    SparkAIAnimation(QString::fromStdString(res));
                } else if (type==2) {
                    SparkAIMusic(QString::fromStdString(res));
                }
            }, Qt::QueuedConnection);
        });
    });
    
}

void VideoBox::SparkAIEmotion(QString res)
{
    QJsonDocument json = QJsonDocument::fromJson(res.toUtf8());
    if (json.isNull()) {
        qDebug() << "json error";
    }
    clearMark();
    for (const auto &item : json.object()["faces"].toArray()) {
        QJsonObject obj = item.toObject();
        addMark(obj["left"].toInt(),
                obj["top"].toInt(),
                obj["width"].toInt(),
                obj["height"].toInt(),
                obj["expression"].toString());
    }
    showMark();
}

void VideoBox::SparkAIAnimation(QString res)
{
    QJsonDocument json = QJsonDocument::fromJson(res.toUtf8());
    if (json.isNull()) {
        qDebug() << "json error";
    }
    qDebug() << "animation:" << res;
    clearMark();
    QSize size = SparkMediaControler::getInstance()->getImg()->size();
    for (const auto &item : json.object()["animation_data"].toArray()) {
        QJsonObject obj = item.toObject();
        int x1 = obj["box"].toArray()[0].toDouble()*size.width();
        int y1 = obj["box"].toArray()[1].toDouble()*size.height();
        int x2 = obj["box"].toArray()[2].toDouble()*size.width();
        int y2 = obj["box"].toArray()[3].toDouble()*size.height();
        addMark(x1,
                y1,
                x2-x1,
                y2-y1,
                obj["character"].toArray()[0].toObject()["character"].toString()+"\n"+obj["character"].toArray()[0].toObject()["work"].toString());
    }
    showMark();
}

void VideoBox::SparkAIMusic(QString res)
{
    QJsonDocument json = QJsonDocument::fromJson(res.toUtf8());
    if (json.isNull()) {
        qDebug() << "json error";
    }
    QJsonArray array = json.object()["tags"].toArray();
    QString str = "";
    for (const auto &item : array) {
        str += item.toString() + ", ";
    }
    str =  str=="" ? "识别失败,换一首试试吧" : str;
    QPixmap orig = QPixmap::fromImage(*(SparkMediaControler::getInstance()->getImg()));
    if (orig.isNull())
        return;

    QPixmap overlay = orig.copy();
    QPainter painter(&overlay);
    painter.setRenderHint(QPainter::Antialiasing);
    //调整字体大小
    QFont font = painter.font();
    font.setPointSize(25);
    painter.setFont(font);
    QFontMetrics fm(painter.font());
    int textHeight = fm.height();
    int textWidth = fm.horizontalAdvance(str);
    //画一个半透明圆角矩形包住文本
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor(255, 255, 255, 128));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(QRect((overlay.width()-textWidth)/2, (overlay.height()-textHeight)/2, textWidth, textHeight), 10, 10);
    
    //在画布中间偏下的位置绘制文本
    painter.setPen(Qt::white);
    QRect textRect((overlay.width()-textWidth)/2, (overlay.height()-textHeight)/2, textWidth, textHeight);
    painter.drawText(textRect, Qt::AlignCenter, str);
    painter.end();
    AIimg = new QPixmap(overlay);
    showimg();
}

void VideoBox::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        if (!is_fullscreen)
        {
            fullscreen(true);
        }
        
    }
}

void VideoBox::mouseMoveEvent(QMouseEvent *ev)
{
    this->setCursor(Qt::ArrowCursor);
    hideMouseTimer->start(); // 重启计时器
    QPoint pos = ev->pos();
    if (pos.y()<50 || pos.y()>window()->height()-80) {
        emit onNeedShow();
    } else {
        emit onNeedHide();
    }
    
}

void VideoBox::resizeEvent()
{
    if (is_fullscreen)
    {
        this->resize(parentWidget()->size());
        showimg();
    } else {
        this->move(10,parentWidget()->height()-50-5);
    }
}

void VideoBox::showimg()
{
    if (is_fullscreen)
    {
        if (AIimg)
        {
            setPixmap(ImageTools::toPixmap(AIimg->toImage(),QSize(this->width(),this->height())));                   
        } else
        {
            setPixmap(ImageTools::toPixmap(*(SparkMediaControler::getInstance()->getImg()),QSize(this->width(),this->height())));
        }
        
        
    } else {
        setPixmap(ImageTools::toPixmap(*(SparkMediaControler::getInstance()->getImg()),QSize(this->width(),this->height()),5));
    }

}

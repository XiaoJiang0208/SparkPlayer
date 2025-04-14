#include "PlayListBox.h"

PlayListBox::PlayListBox(QWidget *parent):QLabel(parent)
{
    initUI();

    connect(SparkMediaControler::getInstance(),&SparkMediaControler::onPlayListChange,this,&PlayListBox::slotReloadPlayList);

    connect(DGuiApplicationHelper::instance(),&DGuiApplicationHelper::themeTypeChanged,this,&PlayListBox::slotThemeTypeChanged);
    slotThemeTypeChanged();
}

PlayListBox::~PlayListBox()
{
}

void PlayListBox::initUI()
{
    this->setLayout(new QVBoxLayout(this)); //设置布局
    this->layout()->setContentsMargins(0,10,0,10);
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
    media_list_context_layout->setContentsMargins(0, 0, 0, 0);
    media_list_context_layout->setSpacing(0);
    media_list_context_layout->setAlignment(Qt::AlignTop);  // 新增，确保从上到下排列
    
    title = new QLabel("播放列表\n右键从播放列表删除",media_list_context);
    title->setAlignment(Qt::AlignCenter);
    media_list_context_layout->addWidget(title);
    media_list->setWidget(media_list_context);
}

void PlayListBox::slotReloadPlayList()
{
    std::deque<MediaBox *>::iterator b = media_buttons.begin();
    // TODO 优化
    while (!media_buttons.empty())
    {
        media_buttons.front()->deleteLater();
        media_buttons.pop_front();

    }
    for (auto d : *(SparkMediaControler::getInstance()->getPlayList())) {
        MediaBox *b = new MediaBox(d,media_list_context,true);
        b->setFixedHeight(40);
        media_list_context_layout->addWidget(b);
        b->autoSetIcon();
        media_buttons.push_back(b);
        // media_buttons.push_back(new MediaBox(d,media_list_context,true));
        // media_buttons.back()->setFixedHeight(40);
        // media_list_context_layout->addWidget(media_buttons.back());
        // media_buttons.back()->autoSetIcon();
    }
}

void PlayListBox::slotThemeTypeChanged() {
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(30);    // 模糊半径
    shadow->setOffset(0, 0 );        // 偏移量
    shadow->setColor(qRgb(51,51,51));    // 阴影颜色
    this->setGraphicsEffect(shadow);
    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType)
    {
        this->setStyleSheet(".PlayListBox{ background-color: #fafafa; border-top-left-radius: 10px; border-bottom-left-radius: 10px; }");
        media_list_context->setStyleSheet(".QWidget{background-color: #fafafa; border-top-left-radius: 10px; border-bottom-left-radius: 10px; }");
        media_list->setStyleSheet(".QScrollArea{background-color: #fafafa;border-style: none; border-top-left-radius: 10px; border-bottom-left-radius: 10px; }");

    } else {
        this->setStyleSheet(".PlayListBox{ background-color: #1a1a21; border-top-left-radius: 10px; border-bottom-left-radius: 10px; }");
        media_list_context->setStyleSheet(".QWidget{background-color: #1a1a21; border-top-left-radius: 10px; border-bottom-left-radius: 10px; }");
        media_list->setStyleSheet(".QScrollArea{background-color: #1a1a21;border-style: none; border-top-left-radius: 10px; border-bottom-left-radius: 10px; }");
    }
}

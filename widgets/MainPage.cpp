#include "MainPage.h"

MainPage::MainPage(QWidget *parent, const PageData &data)
    : DWidget(parent)
{
    this->data = data;
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &MainPage::slotThemeTypeChanged);
    initUI();
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
    
    //初始化主标题栏
    media_list_bar = new DWidget(media_list_context);
    media_list_bar->setFixedHeight(100);
    media_list_bar_layout = new QHBoxLayout(media_list_bar);
    media_list_bar_layout->setAlignment(Qt::AlignLeft);

    media_list_context_layout->addWidget(media_list_bar);

    media_list_bar_icon = new DLabel(media_list_bar);
    media_list_bar_icon->setPixmap(ImageTools::toPixmap(Path::applicationPath("images/bg.png").toString(), QSize(100, 80),15));
    media_list_bar_icon->show();
    media_list_bar_layout->addWidget(media_list_bar_icon);

    media_list_bar_title = new DLabel(data.title,media_list_bar);
    media_list_bar_title->setFont(DFontSizeManager::instance()->t3());
    media_list_bar_title->show(); 
    media_list_bar_layout->addWidget(media_list_bar_title);


    for (int i = 0; i < 3; i++)
    {
        DPushButton *b = new DPushButton(QString("button%1").arg(i),media_list_context);
        b->setFixedHeight(50);
        b->setIcon(b->style()->standardIcon(DStyle::SP_ComputerIcon));
        media_list_context_layout->addWidget(b);
    }
    
    media_list->setWidget(media_list_context);

    slotThemeTypeChanged();
}

void MainPage::slotThemeTypeChanged()
{ 
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
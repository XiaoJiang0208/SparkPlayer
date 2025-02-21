#include "MainPage.h"

MainPage::MainPage(QWidget *parent)
    : DWidget(parent)
{
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
    media_list->layout()->addWidget(media_list_context);
    media_list_context_layout = new QVBoxLayout(media_list_context);
    media_list_context_layout->setMargin(0);
    media_list_context_layout->setSpacing(0);
    media_list_context_layout->setAlignment(Qt::AlignTop);  // 新增，确保从上到下排列
    
    //初始化主标题栏
    media_list_bar = new DWidget(media_list_context);
    media_list_bar_layout = new QVBoxLayout(media_list_bar);
    media_list_bar->setFixedHeight(100);
    media_list_bar_title = new DLabel(tr("MainPage"),media_list_bar);
    media_list_bar_layout->addWidget(media_list_bar_title);
    media_list_context_layout->addWidget(media_list_bar);
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

    media_list->setStyleSheet(".QScrollArea{border-style: none;background-color:rgba(0, 0, 0, 0);}");
    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType){
        media_list_bar->setStyleSheet(".QWidget{background-color:rgb(28, 89, 150);}");
    } else {

    }
    // Implement the logic for theme type change
}
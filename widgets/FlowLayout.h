#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H

#include <QLayout>
#include <DWidget>
#include <DStyle>

DWIDGET_USE_NAMESPACE

class FlowLayout : public QLayout
{
public:
    /**
     * @brief 构造函数，创建一个带有父窗口的流布局
     * @param parent 父窗口
     * @param margin 布局的边距
     * @param hSpacing 水平间距
     * @param vSpacing 垂直间距
     */
    explicit FlowLayout(QWidget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);

    /**
     * @brief 构造函数，创建一个不带父窗口的流布局
     * @param margin 布局的边距
     * @param hSpacing 水平间距
     * @param vSpacing 垂直间距
     */
    explicit FlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);

    /**
     * @brief 析构函数，销毁流布局
     */
    ~FlowLayout();

    /**
     * @brief 添加布局项
     * @param item 要添加的布局项
     */
    void addItem(QLayoutItem *item) override;

    /**
     * @brief 获取水平间距
     * @return 水平间距
     */
    int horizontalSpacing() const;

    /**
     * @brief 获取垂直间距
     * @return 垂直间距
     */
    int verticalSpacing() const;

    /**
     * @brief 获取扩展方向
     * @return 扩展方向
     */
    Qt::Orientations expandingDirections() const override;

    /**
     * @brief 检查是否有高度宽度
     * @return 如果有高度宽度则返回true，否则返回false
     */
    bool hasHeightForWidth() const override;

    /**
     * @brief 获取指定宽度的高度
     * @param width 指定的宽度
     * @return 对应的高度
     */
    int heightForWidth(int width) const override;

    /**
     * @brief 获取布局项的数量
     * @return 布局项的数量
     */
    int count() const override;

    /**
     * @brief 获取指定索引处的布局项
     * @param index 布局项的索引
     * @return 指定索引处的布局项
     */
    QLayoutItem *itemAt(int index) const override;

    /**
     * @brief 获取最小尺寸
     * @return 最小尺寸
     */
    QSize minimumSize() const override;

    /**
     * @brief 设置布局的几何形状
     * @param rect 布局的几何形状
     */
    void setGeometry(const QRect &rect) override;

    /**
     * @brief 获取推荐的尺寸
     * @return 推荐的尺寸
     */
    QSize sizeHint() const override;

    /**
     * @brief 移除并返回指定索引处的布局项
     * @param index 布局项的索引
     * @return 移除的布局项
     */
    QLayoutItem *takeAt(int index) override;

private:
    /**
     * @brief 执行布局
     * @param rect 布局的矩形区域
     * @param testOnly 如果为true，则仅测试布局而不实际设置几何形状
     * @return 布局的高度
     */
    int doLayout(const QRect &rect, bool testOnly) const;

    /**
     * @brief 获取智能间距
     * @param pm 像素度量
     * @return 智能间距
     */
    int smartSpacing(QStyle::PixelMetric pm) const;

    QList<QLayoutItem *> itemList; ///< 布局项列表
    int m_hSpace; ///< 水平间距
    int m_vSpace; ///< 垂直间距
};

#endif // FLOWLAYOUT_H
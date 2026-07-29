#pragma once

#include <QQuickItem>
#include <QPointer>
#include <QColor>
#include <QSizeF>


namespace qan {

class Node;
class Graph;
class NodeStyle;
// Phase 2+：可视化的节点图形项
//
// TODO:
// - 继承 QQuickItem
// - QML_ELEMENT
// - 属性：x, y, width, height, minimumSize
// - 鼠标事件：拖拽移动
// - 样式属性：style（颜色、边框等）
// - 指向 qan::Node 的 _node 指针

class NodeItem : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT
    // TODO: Phase 2 开始实现
public:
    explicit NodeItem(QQuickItem* parent = nullptr);
    virtual ~NodeItem() override=default;
    NodeItem(const NodeItem&) = delete;

    Q_PROPERTY(qan::Node* node READ getNode CONSTANT)
    Node* getNode() noexcept {return _node;}
    void setNode(Node* node) noexcept {_node = node;}
private:
    QPointer<Node> _node;

    Q_PROPERTY(QColor backColor READ getBackColor WRITE setBackColor NOTIFY backColorChanged FINAL)
    QColor getBackColor() const noexcept {return _backColor;}
    void setBackColor(const QColor& color) noexcept {_backColor = color;update();}
signals:
    void backColorChanged();

private:
    QColor _backColor{ "white"};

    Q_PROPERTY(QColor borderColor READ getBorderColor WRITE setBorderColor NOTIFY borderColorChanged FINAL)
    QColor getBorderColor() const noexcept {return _borderColor;}
    void setBorderColor(const QColor& color) noexcept {_borderColor = color;update();}
signals:
    void borderColorChanged();
private:
    QColor _borderColor{ "black"};

    Q_PROPERTY(qreal borderWidth READ getBorderWidth WRITE setBorderWidth NOTIFY borderWidthChanged FINAL)
    qreal getBorderWidth() const noexcept {return _borderWidth;}
    void setBorderWidth(qreal width) noexcept {_borderWidth = width;update();}
signals:
    void borderWidthChanged();
private:
    qreal _borderWidth{ 1.0 };

    Q_PROPERTY(bool fontBold READ getFontBold WRITE setFontBold NOTIFY fontBoldChanged FINAL)
    bool getFontBold() const noexcept {return _fontBold;}
    void setFontBold(bool bold) noexcept {_fontBold = bold;update();}
signals:
    void fontBoldChanged();
private:
    bool _fontBold{ false };

    Q_PROPERTY(QSizeF minimumSize READ getMinimumSize WRITE setMinimumSize NOTIFY minimumSizeChanged FINAL)
    QSizeF getMinimumSize() const noexcept {return _minimumSize;}
    void setMinimumSize(const QSizeF size) noexcept {_minimumSize = size;}
signals:
    void minimumSizeChanged();
private:
    QSizeF _minimumSize{ 100.0, 40.0 };

    Q_PROPERTY(NodeStyle* style READ getStyle WRITE setStyle NOTIFY styleChanged FINAL)
    NodeStyle* getStyle() noexcept {return _style;}
    void setStyle(NodeStyle* style);
signals:
    void styleChanged();
private:
    QPointer<NodeStyle> _style;

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

protected:
    QSGNode* updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData* data) override;
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;

private:
    QPointF  _dragStartPos;
    bool  _dragging{ false };    
} // ::qan

QML_DECLARE_TYPE(qan::NodeItem)

#pragma once

#include <QQuickItem>
#include <QPointer>
#include <QColor>
#include <QSizeF>


namespace qan {

class Node;
class Graph;
class NodeStyle;
// ============================================================================
// NodeItem.h — qan::NodeItem 精简参考实现
// ============================================================================
//
// 可视化节点：一个可拖拽的矩形 + 标签文字。
//
// 核心功能：
//   1) 显示一个圆角矩形（背景色、边框、标签）
//   2) 支持鼠标拖拽移动
//   3) 双击/右键事件传递到 qan::Node 的信号
//   4) 绑定 qan::NodeStyle（如果存在）来改变外观
//
// 属性：
//   - minimumSize : 节点最小尺寸（防止内容过小无法操作）
//
// 绘制在 QQuickItem::paint() 中（QPainter），不使用子 QML Item。
// ============================================================================

class NodeItem : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT
    // TODO: Phase 2 开始实现
public:
    explicit NodeItem(QQuickItem* parent = nullptr);
    virtual ~NodeItem() override=default;
    NodeItem(const NodeItem&) = delete;

    // ── 关联的节点数据模型 ──
    Q_PROPERTY(qan::Node* node READ getNode CONSTANT)
    Node* getNode() noexcept {return _node;}
    void setNode(Node* node) noexcept {_node = node;}
private:
    QPointer<Node> _node;

    // ── 样式（暂时用属性直写，等实现 NodeStyle 后替换）──
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

    // ── 最小尺寸 ──
    Q_PROPERTY(QSizeF minimumSize READ getMinimumSize WRITE setMinimumSize NOTIFY minimumSizeChanged FINAL)
    QSizeF getMinimumSize() const noexcept {return _minimumSize;}
    void setMinimumSize(const QSizeF size) noexcept {_minimumSize = size;}
signals:
    void minimumSizeChanged();
private:
    QSizeF _minimumSize{ 100.0, 40.0 };

    // ── 样式对象（可选，后续对接 NodeStyle）──
    Q_PROPERTY(NodeStyle* style READ getStyle WRITE setStyle NOTIFY styleChanged FINAL)
    NodeStyle* getStyle() noexcept {return _style;}
    void setStyle(NodeStyle* style);
signals:
    void styleChanged();
private:
    QPointer<NodeStyle> _style;

    // ── 拖拽 ──
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    // ── 绘制 ──
protected:
    QSGNode* updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData* data) override;
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;

    // ── 拖拽状态 ──
private:
    QPointF  _dragStartPos;
    bool  _dragging{ false };    
} // ::qan

QML_DECLARE_TYPE(qan::NodeItem)
